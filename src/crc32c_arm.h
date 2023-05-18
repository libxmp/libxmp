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

#ifndef XMP_CRC32C_ARM_H
#define XMP_CRC32C_ARM_H

/**
 * ARM hardware CRC supporting the following methods:
 * - CRC extension (ARMv8 AArch64 and AArch32, mandatory in ARMv8.1+).
 * - NEON AES extension (ARMv8 AArch64 and AArch32, optional).
 * Fragment for crc32c.c, should not be included elsewhere.
 *
 * TODO: MSVC >= 1920(?) supports all intrinsics, but how to runtime detect?
 * TODO: RISC OS GCC predates arm_acle.h
 * TODO: might be able to support older GCC and clang by avoiding poly64x2_t.
 * TODO: would be nice to be able to compile this without explicit -march,
 *       which probably requires inline asm.
 */

#if (defined(__ARM_FEATURE_CRC32) && defined(HAVE_ARM_ACLE_H))

#include <arm_acle.h>

/* GCC: vaddq_p64 added in 10.2 */
#if defined(__GNUC__) &&\
	(__GNUC__ >= 11 || (__GNUC__ >= 10 && __GNUC_MINOR__ >= 2)) &&\
	(defined(__ARM_FEATURE_AES) || defined(__ARM_FEATURE_CRYPTO))
#define LIBXMP_NEON
#endif

/* clang: vaddq_p64 added in 12.0 */
#if defined(__clang__) && defined(__clang_major__) && __clang_major__ >= 12 &&\
	(defined(__ARM_FEATURE_AES) || defined(__ARM_FEATURE_CRYPTO))
#define LIBXMP_NEON
#endif

#ifdef LIBXMP_NEON
#include <arm_neon.h>
#endif

/* Mac OS processor feature detection header. */
#ifdef HAVE_SYSCTLBYNAME
#include <sys/sysctl.h>
#endif
/* Linux processor feature detection headers. */
#ifdef HAVE_GETAUXVAL
#include <sys/auxv.h>
#endif
#ifdef HAVE_ASM_HWCAP_H
#include <asm/hwcap.h>
#endif

#ifndef HWCAP_PMULL
#define HWCAP_PMULL 0
#endif
#ifndef HWCAP2_PMULL
#define HWCAP2_PMULL 0
#endif

#include "crc32c.h"
#define LIBXMP_HAS_HARDWARE_CRC

static uint32 crc32c_hw_crc(uint32 crc, const void *buf, size_t len)
{
	const uint8 *data = (const uint8 *)buf;
	const uint8 *end;

#ifndef WORDS_BIGENDIAN
	while (len && ((size_t)data & 7) != 0) {
		crc = __crc32cb(crc, *data);
		data++;
		len--;
	}

	end = data + (len - (len & 7));
	while (data < end) {
		crc = __crc32cd(crc, *((uint64 *)data));
		data += 8;
	}
	len &= 7;
#endif

	while (len) {
		crc = __crc32cb(crc, *data);
		data++;
		len--;
	}
	return crc;
}

#ifdef LIBXMP_NEON
static poly64x2_t load(const uint8 *in)
{
	/* Loading u8x16 should be endian-safe. */
	uint8x16_t tmp = vld1q_u8(in);
	return vreinterpretq_p64_u8(tmp);
}

static poly64x2_t load_c(uint64_t lane1, uint64_t lane0)
{
	/* "implicit declaration of function vcreateq_u64" */
	uint64x1_t hi = vcreate_u64(lane1);
	uint64x1_t lo = vcreate_u64(lane0);
	uint64x2_t tmp = vcombine_u64(lo, hi);
	return vreinterpretq_p64_u64(tmp);
}

/* For some reason, the vmull_p64 intrinsic ONLY takes non-vector
 * arguments. Despite this, GCC 10 emits the correct asm here. */
static poly128_t _vmull_low_p64(poly64x2_t src, poly64x2_t mods)
{
	poly64_t src_lane0 = vgetq_lane_p64(src, 0);
	poly64_t mods_lane0 = vgetq_lane_p64(mods, 0);
	return vmull_p64(src_lane0, mods_lane0);
}

static poly64x2_t fold(poly64x2_t src, poly64x2_t mods, poly64x2_t in)
{
	poly128_t a = _vmull_low_p64(src, mods);
	poly128_t b = vmull_high_p64(src, mods);
	in = vaddq_p64(in, vreinterpretq_p64_p128(a));
	in = vaddq_p64(in, vreinterpretq_p64_p128(b));
	return in;
}

static uint32 crc32c_hw_neon(uint32 crc, const void *buf, size_t len)
{
	const uint8 *data = (const uint8 *)buf;
	const uint8 *data2;
	const uint8 *end;

	/* See crc32c.c for explanations of thest constants. */
	const poly64x2_t inv = load_c(0, CRC32C_INVERSE64);
	const poly64x2_t poly = load_c(0, CRC32C_FULL_POLY);
	const poly64x2_t mods447_511 = load_c(0x9e4addf8, 0x740eef02);
	const poly64x2_t mods63_127 = load_c(0x493c7d27, 0xf20c0dfe);
	const poly64x2_t mods31_63 = load_c(0xdd45aab8, 0x493c7d27);
	poly64x2_t p[4];
	poly64x2_t v[4];
	poly64x2_t tmp64x2;
	uint32x4_t tmp32x4;
	poly128_t tmp128;

	v[0] = vaddq_p64(load_c(0, crc), load(data + 0));

	if (len >= 64) {
		v[1] = load(data + 16);
		v[2] = load(data + 32);
		v[3] = load(data + 48);
		data += 64;
		len -= 64;

		/* Reduce to 512 */
		end = data + (len - (len & 63));
		data2 = data;
		while (data < end) {
			p[0] = load(data + 0);
			v[0] = fold(v[0], mods447_511, p[0]);
			p[1] = load(data + 16);
			v[1] = fold(v[1], mods447_511, p[1]);
			p[2] = load(data + 32);
			v[2] = fold(v[2], mods447_511, p[2]);
			p[3] = load(data + 48);
			v[3] = fold(v[3], mods447_511, p[3]);
			data += 64;
		}
		len -= data - data2;

		/* Reduce 512 to 128 */
		v[0] = fold(v[0], mods63_127, v[1]);
		v[0] = fold(v[0], mods63_127, v[2]);
		v[0] = fold(v[0], mods63_127, v[3]);
	} else {
		data += 16;
		len -= 16;
	}

	/* Reduce to 128 */
	end = data + (len - (len & 15));
	/*data2 = data;*/
	while (data < end) {
		p[0] = load(data);
		v[0] = fold(v[0], mods63_127, p[0]);
		data += 16;
	}
	/*len -= data - data2;*/

	/* Reduce 128 into 64.
	 * Extract the two most-significant 32-bit words and fold them into
	 * the least-significant 64-bit word (copied from lane 1 to lane 0).
	 * Lane 0 contains the result afterward, lane 1 contains junk.
	 */
	tmp32x4 = vreinterpretq_u32_p64(v[0]);		/* 3 2 1 0 */
	tmp32x4 = vzip1q_u32(tmp32x4, vdupq_n_u32(0));	/* . 1 . 0 */
	tmp64x2 = vreinterpretq_p64_u32(tmp32x4);
	v[0] = vdupq_n_p64(vgetq_lane_p64(v[0], 1));	/* 3 2 3 2 */
	v[0] = fold(tmp64x2, mods31_63, v[0]);

	/* Barrett reduction from 64 to CRC. */
	tmp128 = _vmull_low_p64(v[0], inv);
	tmp128 = _vmull_low_p64(vreinterpretq_p64_p128(tmp128), poly);
	crc = vgetq_lane_u64(vreinterpretq_u64_p128(tmp128), 1);

	return crc;
}
#endif /* LIBXMP_NEON */

static uint32 crc32c_hw(uint32 crc, const void *buf, size_t len, int flags)
{
	const uint8 *data = (const uint8 *)buf;
	crc = ~crc;

#ifdef LIBXMP_NEON
	if ((flags & LIBXMP_ISA_HAS_CLMUL) && len >= 32) {
		/* Align to 16 bytes to avoid cache line boundaries. */
		if (((size_t)data & 15) != 0) {
			size_t sz = MIN(len, ((size_t)data & 15));
			crc = crc32c_hw_crc(crc, data, sz);
			data += sz;
			len -= sz;
		}

		if (len >= 16) {
			crc = crc32c_hw_neon(crc, data, len);
			data += len & ~15;
			len &= 15;
		}
	}
#endif

	if (len) {
		crc = crc32c_hw_crc(crc, data, len);
	}
	return ~crc;
}

static int has_crc32c_hw(void)
{
#if defined(__aarch64__) && defined(HAVE_GETAUXVAL) && defined(HWCAP_CRC32)

	/* Detect via Linux kernel (AArch64). */
	int tmp = getauxval(AT_HWCAP);
	if (~tmp & HWCAP_CRC32)
		return 0;

	return  (tmp & HWCAP_CRC32 ? LIBXMP_ISA_HAS_CRC32 : 0) |
		(tmp & HWCAP_PMULL ? LIBXMP_ISA_HAS_CLMUL : 0);

#elif defined(__arm__) && defined(HAVE_GETAUXVAL) && defined(HWCAP2_CRC32)

	/* Detect via Linux kernel (32-bit). */
	int tmp = getauxval(AT_HWCAP2);
	if (~tmp & HWCAP2_CRC32)
		return 0;

	return	(tmp & HWCAP2_CRC32 ? LIBXMP_ISA_HAS_CRC32 : 0) |
		(tmp & HWCAP2_PMULL ? LIBXMP_ISA_HAS_CLMUL : 0);

#elif defined(__APPLE__) && defined(HAVE_SYSCTLBYNAME)

	/* Detect via Apple kernel */
	size_t val = 0;
	size_t sz = sizeof(val);
	int flags = 0;
	int ret = sysctlbyname("hw.optional.armv8_crc32", &val, &sz, NULL, 0);
	if (ret == 0)
		flags |= val ? LIBXMP_ISA_HAS_CRC32 : 0;

	val = 0;
	ret = sysctlbyname("hw.optional.arm.FEAT_PMULL", &val, &sz, NULL, 0);
	if (ret == 0)
		flags |= val ? LIBXMP_ISA_HAS_CLMUL : 0;

	return flags;

#else
	/* Can't runtime detect at EL0 without a fault :(
	 * https://stackoverflow.com/questions/53965723/how-to-detect-crc32-on-aarch64
	 */
	return 0;
#endif
}

#endif /* ARMv8 */
#endif /* XMP_CRC32C_ARM_H */
