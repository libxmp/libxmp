/* Extended Module Player
 * Copyright (C) 2022 Alice Rowan <petrifiedrowan@gmail.com>
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

/**
 * Unpacker for Amiga LZX compressed streams.
 * Report bugs to libxmp or to here: https://github.com/AliceLR/megazeuxtests
 */

#ifndef LIBXMP_LZX_UNPACK_H
#define LIBXMP_LZX_UNPACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* libxmp hacks */
#include "../common.h"
typedef uint8  lzx_uint8;
typedef uint16 lzx_uint16;
typedef uint32 lzx_uint32;
typedef int32  lzx_int32;

#define LZX_RESTRICT LIBXMP_RESTRICT
/* end libxmp hacks */

enum lzx_method
{
  LZX_M_UNPACKED = 0,
  LZX_M_PACKED   = 2,
  LZX_M_MAX
};

/**
 * Determine if a given LZX method is supported.
 *
 * @param method    compression method to test.
 *
 * @return          0 if a method is supported, otherwise -1.
 */
static inline int lzx_method_is_supported(int method)
{
  switch(method)
  {
    case LZX_M_UNPACKED:
    case LZX_M_PACKED:
      return 0;
  }
  return -1;
}

/**
 * Unpack a buffer containing an LZX compressed stream into an uncompressed
 * representation of the stream. The unpacked method should be handled
 * separately from this function since it doesn't need a second output buffer
 * for the uncompressed data.
 *
 * @param dest        destination buffer for the uncompressed stream.
 * @param dest_len    destination buffer size.
 * @param src         buffer containing the compressed stream.
 * @param src_len     size of the compressed stream.
 * @param method      LZX compression method (should be LZX_M_PACKED).
 *
 * @return            0 on success, otherwise -1.
 */
int lzx_unpack(unsigned char * LZX_RESTRICT dest, size_t dest_len,
 const unsigned char *src, size_t src_len, int method);

#ifdef __cplusplus
}
#endif

#endif /* LIBXMP_LZX_UNPACK_H */
