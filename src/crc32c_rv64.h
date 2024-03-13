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

#ifndef XMP_CRC32C_RV64_H
#define XMP_CRC32C_RV64_H

/**
 * RISC-V 64-bit with Bit Manipulation Extension for CLMUL (Zbc or Zbkc).
 * Fragment for crc32c.c, should not be included elsewhere.
 */

#if defined(__riscv) && __riscv_xlen == 64

#include "crc32c.h"
#define LIBXMP_HAS_HARDWARE_CRC

static uint32 crc32c_hw(uint32 crc, const void *buf, size_t len, int flags)
{
	/* See crc32c.c for explanations of these constants. */
	const uint64 inv	= CRC32C_INVERSE64;
	const uint64 poly	= CRC32C_FULL_POLY;
	const uint64 x31modp	= 0xdd45aab8;
	const uint64 x63modp	= 0x493c7d27;
	const uint64 x127modp	= 0xf20c0dfe;
	const uint8 *data = (const uint8 *)buf;
	const uint8 *end;

	if ((size_t)data & 7) {
		size_t prefix = MIN(len, 8 - ((size_t)data & 7));
		crc = libxmp_crc32c_software(crc, data, prefix);
		data += prefix;
		len -= prefix;
	}

	end = data + (len - (len & 15));
	if (data < end) {
		uint64 hi = ~crc;
		uint64 lo = 0;

		hi ^= *(uint64 *)data;
		lo ^= *(uint64 *)(data + 8);
		data += 16;

		/* Reduce to 128 */
		while (data < end) {
			__asm__("clmul	t0, %0, %2"	"\n\t"
				"clmulh	t1, %0, %2"	"\n\t"
				"clmul	%0, %1, %3"	"\n\t"
				"clmulh	%1, %1, %3"	"\n\t"
				"xor	%0, %0, t0"	"\n\t"
				"xor	%1, %1, t1"	"\n\t"
				: "+r"(hi), "+r"(lo)
				: "r"(x127modp), "r"(x63modp)
				: "t0", "t1");

			hi ^= *(uint64 *)data;
			lo ^= *(uint64 *)(data + 8);
			data += 16;
		}
		/* Reduce to 64 */
		__asm__("li	t0, 0xffffffff"		"\n\t"
			"and	t0, %1, t0"		"\n\t"
			"srl	t1, %1, 32"		"\n\t"
			"clmul	t0, t0, %2"		"\n\t"
			"clmul	t1, t1, %3"		"\n\t"
			"xor	%0, %0, t0"		"\n\t"
			"xor	%0, %0, t1"		"\n\t"
			: "+r"(lo)
			: "r"(hi), "r"(x63modp), "r"(x31modp)
			: "t0", "t1");

		/* Barrett reduction from 64 bits to a 32-bit remainder. */
		/* clmulr with POLY is equivalent to clmulh with FULL_POLY.
		 * Zbkc does not have clmulr, so prefer the clmulh variant. */
		__asm__("clmul	t0, %0, %1"		"\n\t"
			"clmulh	%0, t0, %2"		"\n\t"
			: "+r"(lo)
			: "r"(inv), "r"(poly)
			: "t0");

		crc = ~lo;
	}

	len &= 15;
	if (len) {
		crc = libxmp_crc32c_software(crc, data, len);
	}

	return crc;
}

static int has_crc32c_hw(void)
{
	/* TODO: like ARM, the register for this doesn't work in user mode :(
	 * Linux has not added HWCAP flags for the B extensions yet. */
#if defined(__riscv_bitmanip) || defined(__riscv_b) || defined(__riscv_zbc) || defined(__riscv_zbkc)
	return LIBXMP_ISA_HAS_CLMUL;
#else
	return 0;
#endif
}

#endif /* __riscv */
#endif /* XMP_CRC32C_RV64_H */
