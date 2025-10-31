/* Extended Module Player
 * Copyright (C) 2024-2025 Alice Rowan <petrifiedrowan@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Depacker for Pack-Ice Ice!/ICE! packed files.
 * Due to the strange reverse output nature of this format
 * it has to be depacked in memory all at once.
 *
 * Implementation largely based on this post by nocash:
 * https://eab.abime.net/showpost.php?p=1617809&postcount=7
 */

#include "ice_unpack.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Enable debug output to stderr. */
#if 0
#define ICE_DEBUG
#include <inttypes.h> /* PRId64 */
#endif

/* Enable the original bitstream, which is slower than the replacement.
 * It is provided for reference. */
#if 0
#define ICE_ORIGINAL_BITSTREAM
#endif

/* Enable 64-bit integer bitplanes decoding, which is much faster than the
 * version of the carry flag based bitplane decoder implemented here.
 * This is faster even on most 32-bit hardware. */
#if 1
#define ICE_FAST_BITPLANES
#endif

/* Size of input buffer for filesystem reads. */
#define ICE_BUFFER_SIZE		4096

/* loader.h */
#ifndef MAGIC4
#define MAGIC4(a,b,c,d) \
	(((ice_uint32)(a)<<24)|((ice_uint32)(b)<<16)|((ice_uint32)(c)<<8)|(d))
#endif

#define ICE_OLD_MAGIC		MAGIC4('I','c','e','!')
#define ICE_NEW_MAGIC		MAGIC4('I','C','E','!')
#define CJ_MAGIC		MAGIC4('-','C','J','-')
#define MICK_MAGIC		MAGIC4('M','I','C','K')
#define SHE_MAGIC		MAGIC4('S','H','E','!')
#define TMM_MAGIC		MAGIC4('T','M','M','!')
#define TSM_MAGIC		MAGIC4('T','S','M','!')

#define VERSION_113		113
#define VERSION_21X		210
#define VERSION_21X_OR_220	215
#define VERSION_220		220
#define VERSION_23X		230

struct ice_state
{
	void *in;
	ice_read_fn read_fn;
	ice_seek_fn seek_fn;
	ice_uint32 compressed_size;
	ice_uint32 uncompressed_size;
	int version;
	int eof;
	ice_uint8 buffer[ICE_BUFFER_SIZE + 4];
	unsigned buffer_pos;
	unsigned next_length;
	ice_int64 next_seek;
};

#if (defined(__GNUC__) || defined(__clang__)) && !defined(ICE_DEBUG)
#define debug(...)
#else
ICE_ATTRIB_PRINTF(1,2)
static inline void debug(const char *fmt, ...)
{
#ifdef ICE_DEBUG
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	fflush(stderr);
	va_end(args);
#else
	(void)fmt;
#endif
}
#endif

#ifndef ICE_ORIGINAL_BITSTREAM
static ICE_INLINE ice_uint16 mem_u16le(const ice_uint8 *buf)
{
	return buf[0] | (buf[1] << 8u);
}
#endif

#ifndef ICE_FAST_BITPLANES
static ICE_INLINE ice_uint16 mem_u16be(const ice_uint8 *buf)
{
	return (buf[0] << 8u) | buf[1];
}
#endif

static ICE_INLINE ice_uint32 mem_u32(const ice_uint8 *buf)
{
	return (buf[0] << 24u) | (buf[1] << 16u) | (buf[2] << 8u) | buf[3];
}

#ifndef ICE_FAST_BITPLANES
static ICE_INLINE void put_u16be(ice_uint8 *buf, int val)
{
	buf[0] = (val >> 8) & 0xff;
	buf[1] = val & 0xff;
}
#endif

#ifdef ICE_FAST_BITPLANES
static ICE_INLINE void put_u64be(ice_uint8 *buf, ice_uint64 val)
{
	buf[0] = (val >> 56) & 0xff;
	buf[1] = (val >> 48) & 0xff;
	buf[2] = (val >> 40) & 0xff;
	buf[3] = (val >> 32) & 0xff;
	buf[4] = (val >> 24) & 0xff;
	buf[5] = (val >> 16) & 0xff;
	buf[6] = (val >> 8) & 0xff;
	buf[7] = val & 0xff;
}
#endif

static int ice_check_uncompressed_size(struct ice_state *ice, size_t dest_len)
{
	debug("ice_check_uncompressed_size");
	if (ice->uncompressed_size > dest_len) {
		debug("  uncompressed_size %u exceeds provided buffer size %u\n",
			(unsigned)ice->uncompressed_size, (unsigned)dest_len);
		return -1;
	}
	return 0;
}

static int ice_fill_buffer(struct ice_state *ice, unsigned required)
{
	debug("ice_fill_buffer");
	/* Save up to 4 extra bytes for unaligned reads. */
	if (ice->buffer_pos > 4) {
		debug("  ice_fill_buffer with %u remaining?", ice->buffer_pos);
		return -1;
	}
	if (ice->buffer_pos > 0) {
		memcpy(ice->buffer + ice->next_length, ice->buffer, 4);
	}

	if (ice->seek_fn(ice->in, ice->next_seek, SEEK_SET) < 0) {
#ifdef ICE_DEBUG
		debug("  failed to seek to %" PRId64, ice->next_seek);
#endif
		ice->eof = 1;
		return -1;
	}
	if (ice->read_fn(ice->buffer, ice->next_length, ice->in) < ice->next_length) {
		debug("  failed to read %u", ice->next_length);
		ice->eof = 1;
		return -1;
	}
	ice->buffer_pos += ice->next_length;
	ice->next_seek -= ICE_BUFFER_SIZE;
	ice->next_length = ICE_BUFFER_SIZE;

	if (ice->buffer_pos < required) {
		debug("  less than required after fill: %u < %u",
		 ice->buffer_pos, required);
		return -1;
	}
	return 0;
}

static ICE_INLINE int ice_read_byte(struct ice_state *ice)
{
	if (ice->buffer_pos < 1) {
		if (ice_fill_buffer(ice, 1) < 0)
			return -1;
	}
	return ice->buffer[--ice->buffer_pos];
}

#ifndef ICE_ORIGINAL_BITSTREAM
static ICE_INLINE int ice_read_u16le(struct ice_state *ice)
{
	if (ice->buffer_pos < 2) {
		if (ice_fill_buffer(ice, 2) < 0)
			return -1;
	}
	ice->buffer_pos -= 2;
	return mem_u16le(ice->buffer + ice->buffer_pos);
}
#endif

static ICE_INLINE ice_uint32 ice_peek_u32(struct ice_state *ice)
{
	if (ice->buffer_pos < 4) {
		if (ice_fill_buffer(ice, 4) < 0)
			return 0;
	}
	return mem_u32(ice->buffer + ice->buffer_pos - 4);
}

static int ice_init_buffer(struct ice_state *ice)
{
	ice_uint32 len = ice->compressed_size;
	ice->eof = 0;

	debug("ice_init_buffer");

	ice->next_length = len % ICE_BUFFER_SIZE;
	if (ice->next_length == 0) {
		ice->next_length = ICE_BUFFER_SIZE;
	}
	debug("  initial read length: %u", ice->next_length);

	ice->next_seek = (ice_int64)len - ice->next_length;
	if (ice->version >= VERSION_21X) {
		/* Tracked uncompressed size subtracts the header,
		 * so adjust the seek position to account for it. */
		ice->next_seek += 12;
	}
	debug("  initial read seek: %u", (unsigned)ice->next_seek);

	ice->buffer_pos = 0;
	if (ice_fill_buffer(ice, 1) < 0) {
		return -1;
	}

	/* Attempt version filtering for ambiguous Ice! files.
	 * The initial read of the stream (either 8-bits or 32-bits)
	 * must have the first bit set, indicating an initial literal
	 * string or terminator. When peeking at the first 32-bit big
	 * endian word of the stream, bit 31 will be set for 32-bit
	 * streams and bit 7 will be set for 8-bit streams.
	 *
	 * Because most 32-bit streams do not have bit 7 set (as the
	 * final few bits are usually unused), this disambiguates most
	 * 32-bit streams. 8-bit streams are more likely to have bit 31
	 * set, but this sometimes still disambiguates them. Since 8-bit
	 * streams are far more common and harder to disambiguate, the
	 * unpacking routine for ambiguous files should attempt 8-bit first.
	 */
	if (ice->version == VERSION_21X_OR_220) {
		ice_uint32 peek = ice_peek_u32(ice);
		debug("  version is ambiguous 'Ice!', trying to determine");
		debug("  = %08x", peek);
		if ((~peek & 0x80u) && (peek & 0x80000000u)) {
			debug("  first bit (8bit) not set, must be 32bit");
			ice->version = VERSION_21X;
		} else if ((peek & 0x80u) && (~peek & 0x80000000u)) {
			debug("  first bit (32bit) not set, must be 8bit");
			ice->version = VERSION_220;
		}
	}
	return 0;
}

/* The original Pack-Ice bitstream is implemented roughly as follows:
 *
 * readbit():
 *   bits += bits;                 // add, output is in carry flag
 *   if(!bits)                     // the last bit is a terminating flag
 *     bits = load();
 *     bits += bits + carryflag;   // add-with-carry, output is in carry flag
 *
 * readbits(N):
 *   for 0 until N:
 *     readbit();
 *     out = (out << 1) + carryflag;
 *
 * Initially, the Pack-Ice unpacker preloads a byte (or 4 bytes) but
 * does not preload a terminating bit, which means the lowest set bit of
 * the preloaded byte(s) will be used as a terminating bit instead.
 * This function readjusts the initial bit count to reflect this.
 */
static int ice_preload_adjust(
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	unsigned tmp = *bits >> (unsigned)(32 - *bits_left);

	debug("ice_preload_adjust of %02x (bits left: %d)", tmp, *bits_left);

	if (~(*bits) & 0x80000000u) {
		debug("  first bit not set; stream is invalid at this size");
		return -1;
	}
	while (~tmp & 1) {
		tmp >>= 1;
		(*bits_left)--;
	}
	/* Last valid bit is also discarded. */
	tmp >>= 1;
	(*bits_left)--;

	if (*bits_left) {
		tmp <<= 32 - *bits_left;
	}
#ifndef ICE_ORIGINAL_BITSTREAM
	*bits = tmp;
#endif
	debug("  adjusted to %02x (bits left: %d)", *bits, *bits_left);
	return 0;
}

/* Check ice->eof after to detect early end-of-stream. */
static ICE_INLINE void ice_load8(struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	*bits = (unsigned)ice_read_byte(ice) << 24u;
	*bits_left += 8;
}

static ICE_INLINE void ice_load32(struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	*bits = ice_peek_u32(ice);
	*bits_left += 32;
	ice->buffer_pos -= 4;
}

static int ice_bitplane_filter(struct ice_state * ICE_RESTRICT ice,
	ice_uint8 * ICE_RESTRICT dest, size_t dest_len, int stored_size)
{
#ifdef ICE_FAST_BITPLANES
	static const ice_uint64 bit_conv[256] = {
		ICE_UINT64_C(0x0000000000000000), ICE_UINT64_C(0x0000000000000001),
		ICE_UINT64_C(0x0000000000010000), ICE_UINT64_C(0x0000000000010001),
		ICE_UINT64_C(0x0000000100000000), ICE_UINT64_C(0x0000000100000001),
		ICE_UINT64_C(0x0000000100010000), ICE_UINT64_C(0x0000000100010001),
		ICE_UINT64_C(0x0001000000000000), ICE_UINT64_C(0x0001000000000001),
		ICE_UINT64_C(0x0001000000010000), ICE_UINT64_C(0x0001000000010001),
		ICE_UINT64_C(0x0001000100000000), ICE_UINT64_C(0x0001000100000001),
		ICE_UINT64_C(0x0001000100010000), ICE_UINT64_C(0x0001000100010001),
		ICE_UINT64_C(0x0000000000000002), ICE_UINT64_C(0x0000000000000003),
		ICE_UINT64_C(0x0000000000010002), ICE_UINT64_C(0x0000000000010003),
		ICE_UINT64_C(0x0000000100000002), ICE_UINT64_C(0x0000000100000003),
		ICE_UINT64_C(0x0000000100010002), ICE_UINT64_C(0x0000000100010003),
		ICE_UINT64_C(0x0001000000000002), ICE_UINT64_C(0x0001000000000003),
		ICE_UINT64_C(0x0001000000010002), ICE_UINT64_C(0x0001000000010003),
		ICE_UINT64_C(0x0001000100000002), ICE_UINT64_C(0x0001000100000003),
		ICE_UINT64_C(0x0001000100010002), ICE_UINT64_C(0x0001000100010003),
		ICE_UINT64_C(0x0000000000020000), ICE_UINT64_C(0x0000000000020001),
		ICE_UINT64_C(0x0000000000030000), ICE_UINT64_C(0x0000000000030001),
		ICE_UINT64_C(0x0000000100020000), ICE_UINT64_C(0x0000000100020001),
		ICE_UINT64_C(0x0000000100030000), ICE_UINT64_C(0x0000000100030001),
		ICE_UINT64_C(0x0001000000020000), ICE_UINT64_C(0x0001000000020001),
		ICE_UINT64_C(0x0001000000030000), ICE_UINT64_C(0x0001000000030001),
		ICE_UINT64_C(0x0001000100020000), ICE_UINT64_C(0x0001000100020001),
		ICE_UINT64_C(0x0001000100030000), ICE_UINT64_C(0x0001000100030001),
		ICE_UINT64_C(0x0000000000020002), ICE_UINT64_C(0x0000000000020003),
		ICE_UINT64_C(0x0000000000030002), ICE_UINT64_C(0x0000000000030003),
		ICE_UINT64_C(0x0000000100020002), ICE_UINT64_C(0x0000000100020003),
		ICE_UINT64_C(0x0000000100030002), ICE_UINT64_C(0x0000000100030003),
		ICE_UINT64_C(0x0001000000020002), ICE_UINT64_C(0x0001000000020003),
		ICE_UINT64_C(0x0001000000030002), ICE_UINT64_C(0x0001000000030003),
		ICE_UINT64_C(0x0001000100020002), ICE_UINT64_C(0x0001000100020003),
		ICE_UINT64_C(0x0001000100030002), ICE_UINT64_C(0x0001000100030003),
		ICE_UINT64_C(0x0000000200000000), ICE_UINT64_C(0x0000000200000001),
		ICE_UINT64_C(0x0000000200010000), ICE_UINT64_C(0x0000000200010001),
		ICE_UINT64_C(0x0000000300000000), ICE_UINT64_C(0x0000000300000001),
		ICE_UINT64_C(0x0000000300010000), ICE_UINT64_C(0x0000000300010001),
		ICE_UINT64_C(0x0001000200000000), ICE_UINT64_C(0x0001000200000001),
		ICE_UINT64_C(0x0001000200010000), ICE_UINT64_C(0x0001000200010001),
		ICE_UINT64_C(0x0001000300000000), ICE_UINT64_C(0x0001000300000001),
		ICE_UINT64_C(0x0001000300010000), ICE_UINT64_C(0x0001000300010001),
		ICE_UINT64_C(0x0000000200000002), ICE_UINT64_C(0x0000000200000003),
		ICE_UINT64_C(0x0000000200010002), ICE_UINT64_C(0x0000000200010003),
		ICE_UINT64_C(0x0000000300000002), ICE_UINT64_C(0x0000000300000003),
		ICE_UINT64_C(0x0000000300010002), ICE_UINT64_C(0x0000000300010003),
		ICE_UINT64_C(0x0001000200000002), ICE_UINT64_C(0x0001000200000003),
		ICE_UINT64_C(0x0001000200010002), ICE_UINT64_C(0x0001000200010003),
		ICE_UINT64_C(0x0001000300000002), ICE_UINT64_C(0x0001000300000003),
		ICE_UINT64_C(0x0001000300010002), ICE_UINT64_C(0x0001000300010003),
		ICE_UINT64_C(0x0000000200020000), ICE_UINT64_C(0x0000000200020001),
		ICE_UINT64_C(0x0000000200030000), ICE_UINT64_C(0x0000000200030001),
		ICE_UINT64_C(0x0000000300020000), ICE_UINT64_C(0x0000000300020001),
		ICE_UINT64_C(0x0000000300030000), ICE_UINT64_C(0x0000000300030001),
		ICE_UINT64_C(0x0001000200020000), ICE_UINT64_C(0x0001000200020001),
		ICE_UINT64_C(0x0001000200030000), ICE_UINT64_C(0x0001000200030001),
		ICE_UINT64_C(0x0001000300020000), ICE_UINT64_C(0x0001000300020001),
		ICE_UINT64_C(0x0001000300030000), ICE_UINT64_C(0x0001000300030001),
		ICE_UINT64_C(0x0000000200020002), ICE_UINT64_C(0x0000000200020003),
		ICE_UINT64_C(0x0000000200030002), ICE_UINT64_C(0x0000000200030003),
		ICE_UINT64_C(0x0000000300020002), ICE_UINT64_C(0x0000000300020003),
		ICE_UINT64_C(0x0000000300030002), ICE_UINT64_C(0x0000000300030003),
		ICE_UINT64_C(0x0001000200020002), ICE_UINT64_C(0x0001000200020003),
		ICE_UINT64_C(0x0001000200030002), ICE_UINT64_C(0x0001000200030003),
		ICE_UINT64_C(0x0001000300020002), ICE_UINT64_C(0x0001000300020003),
		ICE_UINT64_C(0x0001000300030002), ICE_UINT64_C(0x0001000300030003),
		ICE_UINT64_C(0x0002000000000000), ICE_UINT64_C(0x0002000000000001),
		ICE_UINT64_C(0x0002000000010000), ICE_UINT64_C(0x0002000000010001),
		ICE_UINT64_C(0x0002000100000000), ICE_UINT64_C(0x0002000100000001),
		ICE_UINT64_C(0x0002000100010000), ICE_UINT64_C(0x0002000100010001),
		ICE_UINT64_C(0x0003000000000000), ICE_UINT64_C(0x0003000000000001),
		ICE_UINT64_C(0x0003000000010000), ICE_UINT64_C(0x0003000000010001),
		ICE_UINT64_C(0x0003000100000000), ICE_UINT64_C(0x0003000100000001),
		ICE_UINT64_C(0x0003000100010000), ICE_UINT64_C(0x0003000100010001),
		ICE_UINT64_C(0x0002000000000002), ICE_UINT64_C(0x0002000000000003),
		ICE_UINT64_C(0x0002000000010002), ICE_UINT64_C(0x0002000000010003),
		ICE_UINT64_C(0x0002000100000002), ICE_UINT64_C(0x0002000100000003),
		ICE_UINT64_C(0x0002000100010002), ICE_UINT64_C(0x0002000100010003),
		ICE_UINT64_C(0x0003000000000002), ICE_UINT64_C(0x0003000000000003),
		ICE_UINT64_C(0x0003000000010002), ICE_UINT64_C(0x0003000000010003),
		ICE_UINT64_C(0x0003000100000002), ICE_UINT64_C(0x0003000100000003),
		ICE_UINT64_C(0x0003000100010002), ICE_UINT64_C(0x0003000100010003),
		ICE_UINT64_C(0x0002000000020000), ICE_UINT64_C(0x0002000000020001),
		ICE_UINT64_C(0x0002000000030000), ICE_UINT64_C(0x0002000000030001),
		ICE_UINT64_C(0x0002000100020000), ICE_UINT64_C(0x0002000100020001),
		ICE_UINT64_C(0x0002000100030000), ICE_UINT64_C(0x0002000100030001),
		ICE_UINT64_C(0x0003000000020000), ICE_UINT64_C(0x0003000000020001),
		ICE_UINT64_C(0x0003000000030000), ICE_UINT64_C(0x0003000000030001),
		ICE_UINT64_C(0x0003000100020000), ICE_UINT64_C(0x0003000100020001),
		ICE_UINT64_C(0x0003000100030000), ICE_UINT64_C(0x0003000100030001),
		ICE_UINT64_C(0x0002000000020002), ICE_UINT64_C(0x0002000000020003),
		ICE_UINT64_C(0x0002000000030002), ICE_UINT64_C(0x0002000000030003),
		ICE_UINT64_C(0x0002000100020002), ICE_UINT64_C(0x0002000100020003),
		ICE_UINT64_C(0x0002000100030002), ICE_UINT64_C(0x0002000100030003),
		ICE_UINT64_C(0x0003000000020002), ICE_UINT64_C(0x0003000000020003),
		ICE_UINT64_C(0x0003000000030002), ICE_UINT64_C(0x0003000000030003),
		ICE_UINT64_C(0x0003000100020002), ICE_UINT64_C(0x0003000100020003),
		ICE_UINT64_C(0x0003000100030002), ICE_UINT64_C(0x0003000100030003),
		ICE_UINT64_C(0x0002000200000000), ICE_UINT64_C(0x0002000200000001),
		ICE_UINT64_C(0x0002000200010000), ICE_UINT64_C(0x0002000200010001),
		ICE_UINT64_C(0x0002000300000000), ICE_UINT64_C(0x0002000300000001),
		ICE_UINT64_C(0x0002000300010000), ICE_UINT64_C(0x0002000300010001),
		ICE_UINT64_C(0x0003000200000000), ICE_UINT64_C(0x0003000200000001),
		ICE_UINT64_C(0x0003000200010000), ICE_UINT64_C(0x0003000200010001),
		ICE_UINT64_C(0x0003000300000000), ICE_UINT64_C(0x0003000300000001),
		ICE_UINT64_C(0x0003000300010000), ICE_UINT64_C(0x0003000300010001),
		ICE_UINT64_C(0x0002000200000002), ICE_UINT64_C(0x0002000200000003),
		ICE_UINT64_C(0x0002000200010002), ICE_UINT64_C(0x0002000200010003),
		ICE_UINT64_C(0x0002000300000002), ICE_UINT64_C(0x0002000300000003),
		ICE_UINT64_C(0x0002000300010002), ICE_UINT64_C(0x0002000300010003),
		ICE_UINT64_C(0x0003000200000002), ICE_UINT64_C(0x0003000200000003),
		ICE_UINT64_C(0x0003000200010002), ICE_UINT64_C(0x0003000200010003),
		ICE_UINT64_C(0x0003000300000002), ICE_UINT64_C(0x0003000300000003),
		ICE_UINT64_C(0x0003000300010002), ICE_UINT64_C(0x0003000300010003),
		ICE_UINT64_C(0x0002000200020000), ICE_UINT64_C(0x0002000200020001),
		ICE_UINT64_C(0x0002000200030000), ICE_UINT64_C(0x0002000200030001),
		ICE_UINT64_C(0x0002000300020000), ICE_UINT64_C(0x0002000300020001),
		ICE_UINT64_C(0x0002000300030000), ICE_UINT64_C(0x0002000300030001),
		ICE_UINT64_C(0x0003000200020000), ICE_UINT64_C(0x0003000200020001),
		ICE_UINT64_C(0x0003000200030000), ICE_UINT64_C(0x0003000200030001),
		ICE_UINT64_C(0x0003000300020000), ICE_UINT64_C(0x0003000300020001),
		ICE_UINT64_C(0x0003000300030000), ICE_UINT64_C(0x0003000300030001),
		ICE_UINT64_C(0x0002000200020002), ICE_UINT64_C(0x0002000200020003),
		ICE_UINT64_C(0x0002000200030002), ICE_UINT64_C(0x0002000200030003),
		ICE_UINT64_C(0x0002000300020002), ICE_UINT64_C(0x0002000300020003),
		ICE_UINT64_C(0x0002000300030002), ICE_UINT64_C(0x0002000300030003),
		ICE_UINT64_C(0x0003000200020002), ICE_UINT64_C(0x0003000200020003),
		ICE_UINT64_C(0x0003000200030002), ICE_UINT64_C(0x0003000200030003),
		ICE_UINT64_C(0x0003000300020002), ICE_UINT64_C(0x0003000300020003),
		ICE_UINT64_C(0x0003000300030002), ICE_UINT64_C(0x0003000300030003)
	};
	ice_uint64 planes;
#endif
	ice_uint8 *pos;
	ice_uint8 *end;

	if (stored_size < 0 || (size_t)stored_size * 8 > dest_len) {
		debug("  invalid bitplane length: %d\n", stored_size);
		return -1;
	}

#ifdef ICE_FAST_BITPLANES
	end = dest + dest_len;
	pos = end - (size_t)stored_size * 8;

	/* GCC likes to apply SSE2 vector optimizations to the fast bitplanes
	 * loop that actually make it extremely slow on some P4-era processors;
	 * turn off x86 vectorization. Encountered in its worst form on a
	 * Northwood Celeron 2.6, but it happens to a lesser degree on Atom. */
#if defined(__i386__) && defined(__GNUC__) && __GNUC__ >= 14
#pragma GCC novector
#endif
	for (; pos < end; pos += 8) {
#if defined(__i386__) && defined(__GNUC__) && __GNUC__ < 14
		/* This hack turns off vectorization for older GCC versions. */
		asm("");
#endif
		planes = 0;
		planes |= bit_conv[pos[6]] << 14;
		planes |= bit_conv[pos[7]] << 12;
		planes |= bit_conv[pos[4]] << 10;
		planes |= bit_conv[pos[5]] <<  8;
		planes |= bit_conv[pos[2]] <<  6;
		planes |= bit_conv[pos[3]] <<  4;
		planes |= bit_conv[pos[0]] <<  2;
		planes |= bit_conv[pos[1]];
		put_u64be(pos, planes);
	}
#else
	{
	unsigned plane0, plane1, plane2, plane3;
	unsigned i, j;
	unsigned x;

	pos = dest + dest_len;
	end = pos - (size_t)stored_size * 8;

	plane0 = plane1 = plane2 = plane3 = 0;
	while (pos > end) {
		for (i = 0; i < 4; i++) {
			pos -= 2;
			x = (unsigned)mem_u16be(pos) << 16u;
			for (j = 0; j < 4; j++) {
				plane0 = (plane0 << 1) | (x >> 31u);
				x <<= 1;
				plane1 = (plane1 << 1) | (x >> 31u);
				x <<= 1;
				plane2 = (plane2 << 1) | (x >> 31u);
				x <<= 1;
				plane3 = (plane3 << 1) | (x >> 31u);
				x <<= 1;
			}
		}
		put_u16be(pos + 0, plane0);
		put_u16be(pos + 2, plane1);
		put_u16be(pos + 4, plane2);
		put_u16be(pos + 6, plane3);
	}
	}
#endif
	return 0;
}

/* ice_unpack_fn8 and its helper functions. */
#define STREAMSIZE 8
#include "ice_unpack_fn.c"
#undef STREAMSIZE

/* ice_unpack_fn32 and its helper functions. */
#define STREAMSIZE 32
#include "ice_unpack_fn.c"
#undef STREAMSIZE

static int ice_unpack8(struct ice_state * ICE_RESTRICT ice,
	ice_uint8 * ICE_RESTRICT dest, size_t dest_len)
{
	ice_uint32 bits = 0;
	int bits_left = 0;

	debug("ice_unpack8");
	ice_load8(ice, &bits, &bits_left);
	if (ice->eof || ice_preload_adjust(&bits, &bits_left) < 0) {
		return -1;
	}
	return ice_unpack_fn8(ice, &bits, &bits_left, dest, dest_len);
}

static int ice_unpack32(struct ice_state * ICE_RESTRICT ice,
	ice_uint8 * ICE_RESTRICT dest, size_t dest_len)
{
	ice_uint32 bits = 0;
	int bits_left = 0;

	debug("ice_unpack32");
	ice_load32(ice, &bits, &bits_left);
	if (ice->eof || ice_preload_adjust(&bits, &bits_left) < 0) {
		return -1;
	}
	return ice_unpack_fn32(ice, &bits, &bits_left, dest, dest_len);
}

static int ice_unpack(struct ice_state * ICE_RESTRICT ice,
	ice_uint8 * ICE_RESTRICT dest, size_t dest_len)
{
	debug("ice_unpack");
	/* ice_init_buffer may filter ambiguous versions, so
	 * initialize before entering the unpackX functions. */
	if (ice_init_buffer(ice) < 0) {
		return -1;
	}

	/* If the version is not ambiguous, only a single
	 * unpack for the correct version will be attempted.
	 * Ambiguous files should be unpacked as 8-bit first,
	 * as 32-bit streams are less likely to be ambiguous. */
	if (ice->version >= VERSION_21X_OR_220) {
		if (ice_unpack8(ice, dest, dest_len) == 0) {
			return 0;
		}
	}
	/* Ambiguous version: reset buffer to try again. */
	if (ice->version == VERSION_21X_OR_220 &&
	    ice_init_buffer(ice) < 0) {
		return -1;
	}
	if (ice->version <= VERSION_21X_OR_220) {
		if (ice_unpack32(ice, dest, dest_len) == 0) {
			return 0;
		}
	}
	return -1;
}


ice_int64 ice_uncompressed_bound(ice_uint32 in_len)
{
	/* Best compression: dictionary lookup of length 1033, encoded in
	 * 14 bits, plus a mandatory dictionary distance of 7 bits minimum.
	 * This is an extremely optimistic limit. */
	static const int best_factor = (int)((1033u * 8u + 20u) / (14u + 7u));

	return (ice_int64)in_len * best_factor;
}

ice_int64 ice1_unpack_test(const void *end_of_file, size_t sz)
{
	ice_uint8 *data = (ice_uint8 *)end_of_file;
	ice_uint32 uncompressed_size;
	ice_uint32 magic;

	debug("ice1_unpack_test");
	if (sz < 8) {
		debug("ice1: file too small");
		return -1;
	}
	magic = mem_u32(data + sz - 4);
	uncompressed_size = mem_u32(data + sz - 8);

	if (magic == ICE_OLD_MAGIC) {
		return (ice_int64)uncompressed_size;
	}
	debug("ice1: failed magic check");
	return -1;
}

int ice1_unpack(void * ICE_RESTRICT dest, size_t dest_len,
	ice_read_fn read_fn, ice_seek_fn seek_fn, void *priv, ice_uint32 in_len)
{
	struct ice_state ice;
	ice_uint8 buf[8];

	debug("ice1_unpack");
	memset(&ice, 0, sizeof(ice));

	/* Note: these checks should not fail outside of API misuse. */
	if (in_len < 8) {
		return -1;
	}
	if (seek_fn(priv, -8, SEEK_END) < 0) {
		return -1;
	}
	if (read_fn(buf, 8, priv) < 8) {
		return -1;
	}
	if (ice1_unpack_test(buf, 8) < 0) {
		return -1;
	}

	ice.in = priv;
	ice.read_fn = read_fn;
	ice.seek_fn = seek_fn;
	ice.compressed_size = in_len - 8u;
	ice.uncompressed_size = mem_u32(buf + 0);
	ice.version = VERSION_113;

	if (ice_check_uncompressed_size(&ice, dest_len) < 0) {
		return -1;
	}

	/* Only use the portion of the provided buffer that is needed. */
	return ice_unpack(&ice, (ice_uint8 *)dest, ice.uncompressed_size);
}

ice_int64 ice2_unpack_test(const void *start_of_file, size_t sz)
{
	ice_uint8 *data = (ice_uint8 *)start_of_file;
	ice_uint32 uncompressed_size;
	ice_uint32 magic;

	debug("ice2_unpack_test");
	if (sz < 12) {
		debug("ice2: file too small");
		return -1;
	}
	magic = mem_u32(data + 0);
	uncompressed_size = mem_u32(data + 8);

	switch (magic) {
	case ICE_OLD_MAGIC:
	case ICE_NEW_MAGIC:
	case CJ_MAGIC:
	case MICK_MAGIC:
	case SHE_MAGIC:
	case TMM_MAGIC:
	case TSM_MAGIC:
		return (ice_int64)uncompressed_size;
	}
	debug("ice2: failed magic check");
	return -1;
}

int ice2_unpack(void * ICE_RESTRICT dest, size_t dest_len,
	ice_read_fn read_fn, ice_seek_fn seek_fn, void *priv, ice_uint32 in_len)
{
	struct ice_state ice;
	ice_uint8 buf[12];

	debug("ice2_unpack");
	memset(&ice, 0, sizeof(ice));

	/* Note: these checks should not fail outside of API misuse. */
	if (in_len < 12) {
		return -1;
	}
	if (seek_fn(priv, 0, SEEK_SET) < 0) {
		return -1;
	}
	if (read_fn(buf, 12, priv) < 12) {
		return -1;
	}
	if (ice2_unpack_test(buf, 12) < 0) {
		return -1;
	}

	ice.in = priv;
	ice.read_fn = read_fn;
	ice.seek_fn = seek_fn;
	ice.compressed_size = mem_u32(buf + 4);
	ice.uncompressed_size = mem_u32(buf + 8);

	if (ice.compressed_size != in_len) {
		debug("  bad compressed_size: %u",
			(unsigned)ice.compressed_size);
		return -1;
	}
	ice.compressed_size -= 12;

	if (ice_check_uncompressed_size(&ice, dest_len) < 0) {
		return -1;
	}

	switch (mem_u32(buf + 0)) {
	case ICE_OLD_MAGIC:
		/* Ice! may use a 32-bit or an 8-bit buffer. */
		ice.version = VERSION_21X_OR_220;
		break;
	case ICE_NEW_MAGIC:
		/* ICE! always uses an 8-bit buffer. */
		ice.version = VERSION_23X;
		break;
	default:
		/* Most hacked magics used older versions (apparently). */
		ice.version = VERSION_21X;
		break;
	}

	/* Only use the portion of the provided buffer that is needed. */
	return ice_unpack(&ice, (ice_uint8 *)dest, ice.uncompressed_size);
}
