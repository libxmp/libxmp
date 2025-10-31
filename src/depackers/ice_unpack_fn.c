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

/* Internal code for ice_unpack.c.
 * This file is included twice with different defines to generate the two
 * different depackers, which rely on very different functions to work.
 * The alternative was even more macros... */

#ifndef STREAMSIZE
#error Define STREAMSIZE!
#endif

#undef JOIN_2
#undef JOIN
#define JOIN_2(a, b) a##b
#define JOIN(a, b) JOIN_2(a, b)

#define ice_load_SZ			JOIN(ice_load,STREAMSIZE)
#define ice_read_bits_SZ		JOIN(ice_read_bits,STREAMSIZE)
#define ice_read_literal_length_ext_SZ	JOIN(ice_read_literal_length_ext,STREAMSIZE)
#define ice_read_literal_length_SZ	JOIN(ice_read_literal_length,STREAMSIZE)
#define ice_read_window_length_SZ	JOIN(ice_read_window_length,STREAMSIZE)
#define ice_read_window_distance_SZ	JOIN(ice_read_window_distance,STREAMSIZE)
#define ice_at_stream_start_SZ		JOIN(ice_at_stream_start,STREAMSIZE)
#define ice_unpack_fn_SZ		JOIN(ice_unpack_fn,STREAMSIZE)

static ICE_INLINE int ice_read_bits_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left, int num)
{
	/* NOTE: there are interleaved uncompressed bytes in the input so
	 * this unfortunately can't be optimized very much.
	 */
	int ret = 0;

#ifdef ICE_ORIGINAL_BITSTREAM
	while (num) {
		int bit = (*bits >> 31u);
		*bits <<= 1;
		if (!*bits) {
			ice_load_SZ(ice, bits, bits_left);

			bit = *bits >> 31u;
			*bits = (*bits << 1) | (1 << (32 - STREAMSIZE));
		}
		ret = (ret << 1) | bit;
		num--;
	}
/* !ICE_ORIGINAL_BITSTREAM */
#elif STREAMSIZE == 32
	int left = num - *bits_left;
	ret = *bits >> (32 - num);

	*bits_left -= num;
	if (left <= 0) {
		*bits <<= num;
	} else {
		ice_load_SZ(ice, bits, bits_left);
		debug("        (new buffer %08x)", (unsigned)*bits);
		ret |= *bits >> (32 - left);
		*bits <<= left;
	}
#else /* STREAMSIZE == 8 */
	if (num > *bits_left) {
		if (num > 8 && num - 8 > *bits_left) {
			/* Can load two bytes safely in this case--due to the
			 * backwards stream order they're read little endian. */
			*bits |= (unsigned)ice_read_u16le(ice) << (16u - *bits_left);
			*bits_left += 16;
		} else {
			*bits |= (unsigned)ice_read_byte(ice) << (24u - *bits_left);
			*bits_left += 8;
		}
		debug("        (new buffer %08x)", (unsigned)*bits);
	}
	ret = *bits >> (32 - num);
	*bits <<= num;
	*bits_left -= num;
#endif
	debug("      <- %03x [%d]", ret, num);
	return ret;
}

/* Split off from the main function since 1.x does something else. */
static ICE_INLINE int ice_read_literal_length_ext_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	int length;
#if STREAMSIZE == 32
	if (ice->version == VERSION_113) {
		return ice_read_bits_SZ(ice, bits, bits_left, 10) + 15;
	}
#endif
	length = ice_read_bits_SZ(ice, bits, bits_left,  8) + 15;
	if (length == 270) {
		length = ice_read_bits_SZ(ice, bits, bits_left, 15) + 270;
	}
	return length;
}

static ICE_INLINE int ice_read_literal_length_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	int length = ice_read_bits_SZ(ice, bits, bits_left, 1);
	if (length ==  1) length = ice_read_bits_SZ(ice, bits, bits_left, 1) + 1;
	if (length ==  2) length = ice_read_bits_SZ(ice, bits, bits_left, 2) + 2;
	if (length ==  5) length = ice_read_bits_SZ(ice, bits, bits_left, 2) + 5;
	if (length ==  8) length = ice_read_bits_SZ(ice, bits, bits_left, 3) + 8;
	if (length == 15) length = ice_read_literal_length_ext_SZ(ice, bits, bits_left);
	if (ice->eof) {
		return -1;
	}
	return length;
}

static ICE_INLINE int ice_read_window_length_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left)
{
	int length;
	if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0) {
		length = 2;
	} else if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0) {
		length = 3;
	} else if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0) {
		length = 4 + ice_read_bits_SZ(ice, bits, bits_left, 1);
	} else if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0) {
		length = 6 + ice_read_bits_SZ(ice, bits, bits_left, 2);
	} else {
		length = 10 + ice_read_bits_SZ(ice, bits, bits_left, 10);
	}
	if (ice->eof) {
		return -1;
	}
	debug("    length=%d", length);
	return length;
}

static ICE_INLINE int ice_read_window_distance_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left,
	int length)
{
	int dist;
	if (length == 2) {
		/*
		if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0)
			dist = 1 + ice_read_bits_SZ(ice, bits, bits_left, 6);
		else
			dist = 65 + ice_read_bits_SZ(ice, bits, bits_left, 9);
		*/
		dist = 1 + ice_read_bits_SZ(ice, bits, bits_left, 7);
		if (dist >= 65) {
			dist = ((dist - 65) << 3) + 65 +
				ice_read_bits_SZ(ice, bits, bits_left, 3);
		}
	} else {
		if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0)
			dist = 33 + ice_read_bits_SZ(ice, bits, bits_left, 8);
		else if (ice_read_bits_SZ(ice, bits, bits_left, 1) == 0)
			dist = 1 + ice_read_bits_SZ(ice, bits, bits_left, 5);
		else
			dist = 289 + ice_read_bits_SZ(ice, bits, bits_left, 12);
	}
	if (ice->eof) {
		return -1;
	}
	debug("    dist=%d", dist);
	return dist;
}

static ICE_INLINE int ice_at_stream_start_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left,
	size_t offset)
{
	(void)bits;
	(void)bits_left;

#ifdef ICE_ORIGINAL_BITSTREAM
	return ice->next_seek < 0 && offset == 0 && *bits == 0x80000000u;
#else
	return ice->next_seek < 0 && offset == 0 && *bits_left <= 0;
#endif
}

static ICE_INLINE int ice_unpack_fn_SZ(
	struct ice_state * ICE_RESTRICT ice,
	ice_uint32 * ICE_RESTRICT bits, int * ICE_RESTRICT bits_left,
	ice_uint8 * ICE_RESTRICT dest, size_t dest_len)
{
	ice_uint8 *pos = dest + dest_len;
	ice_uint8 *window_pos;
	int length;
	int dist;

	/* Don't terminate here--streams ending with a window copy expect
	 * a final zero-length literal block. Ending here breaks the
	 * bitplane filter check. */
	while (1) {
		length = ice_read_literal_length_SZ(ice, bits, bits_left);
		if (length < 0) {
			debug("  ERROR: eof decoding literal length");
			return -1;
		}
		debug("  (%zu remaining) copy of %d", pos - dest, length);
		if (length > pos - dest) {
			debug("  ERROR: copy would write past start of file");
			return -1;
		}
		for (; length > 0; length--) {
			int b = ice_read_byte(ice);
			if (b < 0) {
				debug("  ERROR: eof during literal copy");
				return -1;
			}
			*(--pos) = (ice_uint8)b; /* MSVC hallucinates C4244 */
		}
		if (pos == dest) {
			break;
		}

		length = ice_read_window_length_SZ(ice, bits, bits_left);
		if (length <= 0) {
			debug("  ERROR: eof decoding window length");
			return -1;
		}
		dist = ice_read_window_distance_SZ(ice, bits, bits_left, length);
		if (dist <= 0) {
			debug("  ERROR: eof decoding window distance");
			return -1;
		}

		/* The distance value is relative to the last byte written,
		 * not the current position. The copied word never overlaps
		 * the area being written unless dist == 0 (RLE). */
#if STREAMSIZE == 32
		dist = dist + length - 1;
#else
		if (dist > 1) {
			dist = dist + length - 2;
		}
#endif

		debug("  (%zu remaining) window copy of %u, dist %d",
			pos - dest, length, dist);
		if (length > pos - dest) {
			debug("  ERROR: copy would write past start of file");
			return -1;
		}

		window_pos = dist + pos;
		if (window_pos > dest + dest_len) {
			/* This is an out-of-bounds access in the real
			 * Pack-Ice routines, which isn't worth emulating
			 * (does not use a zero-initialized ring buffer
			 * sliding window like DEFLATE). This branch may be
			 * encountered by ambiguous-version "Ice!" streams
			 * when attempting to decode in the wrong mode. */
			debug("  ERROR: copy would read past end of file");
			return -1;
		}
		for (; length > 0; length--) {
			*(--pos) = *(--window_pos);
		}
	}

	/* Bitplane filter (optional). */
	if (ice->version >= VERSION_21X &&
	    ice_read_bits_SZ(ice, bits, bits_left, 1) == 1) {
		debug("  bitplane filter used");
		length = 320 * 200 / 16;
		/* Note: variable length bitplane filtering was added in 2.34(?)
		 * and can only be packed/unpacked with that version, as 2.4
		 * broke filter unpacking/verification entirely. */
		if (!ice_at_stream_start_SZ(ice, bits, bits_left, ice->buffer_pos) &&
		    ice_read_bits_SZ(ice, bits, bits_left, 1) == 1) {
			length = ice_read_bits_SZ(ice, bits, bits_left, 16) + 1;
			if (ice->eof) {
				debug("  failed to read bitplane filter length");
				return -1;
			}
			debug("  bitplane filter of size %d\n", length);
		} else {
			debug("  bitplane filter of size 320 * 200 / 16");
		}

		if (ice_bitplane_filter(ice, dest, dest_len, length) < 0) {
			return -1;
		}
	}

	if (!ice_at_stream_start_SZ(ice, bits, bits_left, ice->buffer_pos)) {
		debug("  ERROR: stream not empty: pos=%u bits=%08x left=%d",
			(unsigned)ice->buffer_pos, (unsigned)*bits, *bits_left);
		return -1;
	}
	return 0;
}

#undef ice_load_SZ
#undef ice_read_bits_SZ
#undef ice_read_literal_length_ext_SZ
#undef ice_read_literal_length_SZ
#undef ice_read_window_length_SZ
#undef ice_read_window_distance_SZ
#undef ice_at_stream_start_SZ
#undef ice_unpack_fn_SZ
