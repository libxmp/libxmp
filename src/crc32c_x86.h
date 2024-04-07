/* crc32c.c -- compute CRC-32C using the Intel crc32 instruction
 * Copyright (C) 2013, 2021 Mark Adler
 * Version 1.2  5 Jun 2021  Mark Adler
 */

/*
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the author be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Mark Adler
  madler@alumni.caltech.edu
 */

/* Copyright (C) 2022, 2023 Alice Rowan <petrifiedrowan@gmail.com>
 * Simplified version of Mark Adler's original CRC-32c implementation.
 * For the original, see: https://stackoverflow.com/a/17646775
 * For the list of modifications, see crc32c.c
 */

#ifndef XMP_CRC32C_X86_H
#define XMP_CRC32C_X86_H

/**
 * Intel x86 with SSE 4.2.
 * Fragment for crc32c.c, should not be included elsewhere.
 *
 * TODO: vectorization with PCLMULDQD
 * TODO: detect cpuid
 * TODO: MSVC, Watcom expect different asm/intrinsics.
 * TODO: crc32q is not supported by 32-bit GCC.
 */

#if defined(__i386) || defined(__i386__) || defined(_M_IX86) || defined(__x86_64__) || defined(_M_AMD64)

#include "crc32c.h"
#define LIBXMP_HAS_HARDWARE_CRC

static uint32 crc32c_hw(uint32 crc, const void *buf, size_t len, int flags)
{
	const uint8 *data = (const uint8 *)buf;
	const uint8 *end;
	uint64 crc0 = ~crc;

	/* Compute the crc for up to seven leading bytes, bringing the data
	 * pointer to an eight-byte boundary. */
	while (len && ((size_t)data & 7) != 0) {
		__asm__("crc32b\t" "(%1), %0"
			: "+r"(crc0)
			: "r"(data), "m"(*data));
		data++;
		len--;
	}

	/* Compute the crc on eight-byte units. */
	end = data + (len - (len & 7));
	while (data < end) {
		__asm__("crc32q\t" "(%1), %0"
			: "+r"(crc0)
			: "r"(data), "m"(*data));
		data += 8;
	}
	len &= 7;

	/* Compute the crc for up to seven trailing bytes. */
	while (len) {
		__asm__("crc32b\t" "(%1), %0"
			: "+r"(crc0)
			: "r"(data), "m"(*data));
		data++;
		len--;
	}

	return ~(uint32)crc0;
}

static int has_crc32c_hw(void)
{
	uint32 eax, ecx;
	int flags;
	eax = 1;
	__asm__("cpuid"
		: "=c"(ecx)
		: "a"(eax)
		: "%ebx", "%edx");

	flags = (ecx & (1 << 20) ? LIBXMP_ISA_HAS_CRC32 : 0) |
		(ecx & (1 <<  1) ? LIBXMP_ISA_HAS_CLMUL : 0);

	/* Require CRC32 for now... */
	return flags & LIBXMP_ISA_HAS_CRC32 ? flags : 0;
}

#endif /* x86 */
#endif /* XMP_CRC32C_X86_H */
