/* Extended Module Player
 * Copyright (C) 2023 Alice Rowan <petrifiedrowan@gmail.com>
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

#ifndef XMP_CRC32C_H
#define XMP_CRC32C_H

#include "common.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order, highest order bit omitted. */
#define CRC32C_POLY		0x82f63b78
/* CRC-32C polynomial in reversed order, with the highest order bit. */
#define CRC32C_FULL_POLY	0x105ec76f1ULL
/* Quotient of x^64 / poly; required for CLMUL-based implementations. */
#define CRC32C_INVERSE64	0x4869ec38dea713f1ULL

#define LIBXMP_ISA_HAS_CRC32 1
#define LIBXMP_ISA_HAS_CLMUL 2

uint32 libxmp_crc32c(uint32, const void *, size_t);
uint32 libxmp_crc32c_software(uint32, const void *, size_t);

#endif /* XMP_CRC32C_H */
