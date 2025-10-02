#include "test.h"

/* Hand-crafted invalid coverage inputs for all Pack-Ice stream types.
 */

TEST(test_fuzzer_depack_ice_coverage)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* Filesize <8 (1.13 only) */
	/* Currently unreachable due to the minimum depacker filesize limit. */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_filesize_113");
	fail_unless(ret == -XMP_ERROR_FORMAT, "filesize_113");

	/* uncompressed_size > LIBXMP_DEPACK_LIMIT (1.13 and 2.x) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_depack_limit_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depack_limit_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_depack_limit_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depack_limit_231");

	/* uncompressed_size > bound (1.13 and 2.x) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_depack_bound_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depack_bound_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_depack_bound_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depack_bound_231");

	/* compressed_size != filesize (1.13 and 2.x) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_compressed_mismatch_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "compressed_mismatch_231");

	/* initial bits 31 and 7 are both 0 (32-bit and 8-bit-ambiguous)
	 * (important: the second test is an ambiguous Ice! header) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_zero_initial_bits_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "zero_initial_bits_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_zero_initial_bits_220");
	fail_unless(ret == -XMP_ERROR_DEPACK, "zero_initial_bits_220");

	/* initial read is 80000000h or 80h, allowing a zero-length copy to
	 * leak through to the depack loop, which should always error.
	 * These files also currently cover the initial read=buffer size case.
	 * (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_zero_initial_copy_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "zero_initial_copy_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_zero_initial_copy_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "zero_initial_copy_231");

	/* eof during literal length read (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_length_eof_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_length_eof_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_length_eof_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_length_eof_231");

	/* eof during literal copy (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_copy_eof_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_copy_eof_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_copy_eof_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_copy_eof_231");

	/* literal copy would write past start of output (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_copy_over_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_copy_over_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_literal_copy_over_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "literal_copy_over_231");

	/* eof during window length read (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_length_eof_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_length_eof_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_length_eof_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_length_eof_231");

	/* eof during window distance read (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_distance_eof_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_distance_eof_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_distance_eof_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_distance_eof_231");

	/* window copy would write past start of output (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_copy_over_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_copy_over_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_copy_over_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_copy_over_231");

	/* window copy would read past end of output (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_copy_oob_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_copy_oob_113");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_window_copy_oob_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "window_copy_oob_231");

	/* eof during bitplane filter length read (32-bit and 8-bit)
	 * This field seems to have been added by 2.34 so the 32-bit
	 * version of this test is slightly anachronistic.
	 * This field should always be present if the bitplane flag
	 * and length flag don't EOF and are both 1. Either of those
	 * flags, however, are allowed to EOF. */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_bitplane_length_eof_211");
	fail_unless(ret == -XMP_ERROR_DEPACK, "bitplane_length_eof_211");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_bitplane_length_eof_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "bitplane_length_eof_231");

	/* bitplane filter length exceeds filesize (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_bitplane_over_211");
	fail_unless(ret == -XMP_ERROR_DEPACK, "bitplane_over_211");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_bitplane_over_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "bitplane_over_231");

	/* extra bits at end of decode (32-bit and 8-bit) */
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_extra_bits_113");
	fail_unless(ret == -XMP_ERROR_DEPACK, "extra_bits_211");
	ret = xmp_load_module(opaque, "data/f/depack_ice_cov_extra_bits_231");
	fail_unless(ret == -XMP_ERROR_DEPACK, "extra_bits_231");

	xmp_free_context(opaque);
}
END_TEST
