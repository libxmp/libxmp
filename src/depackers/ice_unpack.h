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

#ifndef LIBXMP_ICE_UNPACK_H
#define LIBXMP_ICE_UNPACK_H

#ifdef __cplusplus
extern "C" {
#endif

#if 1
/* libxmp hacks */
#include "../common.h"
typedef uint8  ice_uint8;
typedef uint16 ice_uint16;
typedef uint32 ice_uint32;
typedef uint64 ice_uint64;
typedef int64  ice_int64;
#define ICE_RESTRICT		LIBXMP_RESTRICT
#define ICE_ATTRIB_PRINTF(x,y)	LIBXMP_ATTRIB_PRINTF(x,y)
#define ICE_MIN(a,b)		MIN(a,b)
#define ice1_unpack		libxmp_ice1_unpack
#define ice2_unpack		libxmp_ice2_unpack
#define ice1_unpack_test	libxmp_ice1_unpack_test
#define ice2_unpack_test	libxmp_ice2_unpack_test

/* end libxmp hacks */
#else
#include <stdint.h>
typedef uint8_t  ice_uint8;
typedef uint16_t ice_uint16;
typedef uint32_t ice_uint32;
typedef uint64_t ice_uint64;
typedef int64_t  ice_int64;
#define ICE_RESTRICT		__restrict
#define ICE_ATTRIB_PRINTF(x,y)	__attribute__((__format__(printf,x,y)))
#define ICE_MIN(a,b)		((a) < (b) ? (a) : (b))
#endif

#if defined(UINT64_C)
#define ICE_UINT64_C(c) UINT64_C(c)
#elif defined(_MSC_VER)
#define ICE_UINT64_C(c) c ## ui64
#elif defined(__LP64__) || defined(_LP64)
#define ICE_UINT64_C(c) c ## UL
#else
#define ICE_UINT64_C(c) c ## ULL
#endif

/* clang (most recently tested: 20.x) makes really bad inlining decisions
 * in the unpacking code and needs to be encouraged to optimize things
 * correctly (up to 40% improvement measured).
 *
 * Old versions of GCC also benefit, but as of GCC 9.x, the inlining
 * heuristics have improved to the point that performance is actually
 * worse with __attribute__((always_inline)), so limit it.
 *
 * Other compilers have not been tested.
 */
#if defined(__clang__) || \
    (defined(__GNUC__) && \
	(__GNUC__ * 100 + __GNUC_MINOR__ >= 302) && \
	(__GNUC__ < 9))
#define ICE_INLINE		inline __attribute__((always_inline))
#else
#define ICE_INLINE		inline
#endif

#include <stdlib.h>

typedef size_t	(*ice_read_fn)(void * ICE_RESTRICT dest, size_t num, void *priv);
typedef int	(*ice_seek_fn)(void *priv, ice_int64 offset, int whence);

/**
 * Bound the size of an uncompressed Pack-Ice file expanded from a given
 * compressed filesize. This is a very rough upper bound only intended
 * to help filter bad files before allocating a buffer.
 *
 * @param in_len        filesize of input file, including header/footer.
 * @return              maximum possible filesize that could be written.
 */
ice_int64 ice_uncompressed_bound(ice_uint32 in_len);

/**
 * Test the LAST EIGHT BYTES of an input file to determine if it is a
 * Pack-Ice v1 file.
 *
 * @param end_of_file   a buffer containing at least 8 bytes read from the
 *                      END of the file. Only the last eight bytes of this
 *                      buffer will be read. The whole file can be passed here.
 * @param sz            size of `end_of_file`, at least 8.
 * @return              uncompressed size of the stream if Pack-Ice v1, between
 *                      0 and UINT32_MAX, else -1.
 */
ice_int64 ice1_unpack_test(const void *end_of_file, size_t sz);

/**
 * Unpack a Pack-Ice v1 file. Due to the design of the format, the
 * output must be depacked into RAM.
 *
 * @param dest      buffer where the unpacked data should be written.
 * @param dest_len  size of `dest`, should be the return value of `ice1_test`.
 * @param read_fn   function to read `num` bytes from stream `priv`.
 * @param seek_fn   function to seek to `offset` wrt `whence` in stream `priv`.
 *                  Required `whence` values are `SEEK_SET` and `SEEK_END`.
 * @param priv      stream pointer for `read_fn` and `seek_fn`.
 * @param in_len    size of input stream in bytes.
 * @return          0 on success, otherwise -1. `dest` and `dest_len` will be
 *                  written to only if this function returns 0.
 */
int ice1_unpack(void * ICE_RESTRICT dest, size_t dest_len,
 ice_read_fn read_fn, ice_seek_fn seek_fn, void *priv, ice_uint32 in_len);

/**
 * Test the FIRST 12 BYTES of an input file to determine if it is a
 * Pack-Ice v2 file.
 *
 * @param start_of_file   a buffer containing at least 12 bytes read from the
 *                        START of the file. Only the first 12 bytes of this
 *                        buffer will be read. The whole file can be passed here.
 * @param sz              size of `start_of_file`, at least 12.
 * @return                uncompressed size of the stream if Pack-Ice v2, between
 *                        0 and UINT32_MAX, else -1.
 */
ice_int64 ice2_unpack_test(const void *start_of_file, size_t sz);

/**
 * Unpack a Pack-Ice v2 file. Due to the design of the format, the
 * output must be depacked into RAM.
 *
 * @param dest      buffer where the unpacked data should be written.
 * @param dest_len  size of `dest`, should be the return value of `ice2_test`.
 * @param read_fn   function to read `num` bytes from stream `priv`.
 * @param seek_fn   function to seek to `offset` wrt `whence` in stream `priv`.
 *                  Required `whence` values are `SEEK_SET` and `SEEK_END`.
 * @param priv      stream pointer for `read_fn` and `seek_fn`.
 * @param in_len    size of input stream in bytes.
 * @return          0 on success, otherwise -1. `dest` and `dest_len` will be
 *                  written to only if this function returns 0.
 */
int ice2_unpack(void * ICE_RESTRICT dest, size_t dest_len,
 ice_read_fn read_fn, ice_seek_fn seek_fn, void *priv, ice_uint32 in_len);

#ifdef __cplusplus
}
#endif

#endif /* LIBXMP_ICE_UNPACK_H */
