#include "test.h"


TEST(test_fuzzer_prowizard_p40_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused undefined behavior in the ProWizard loader due to
	 * missing checks on the pattern list/patterns/samples base offsets. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p40_invalid_offsets");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_offsets)");

	/* These inputs caused out-of-bounds writes to the track data array in
	 * The Player 4.x depacker due to missing bounds checks on invalid packing. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p40_invalid_packing");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_packing)");

	/* These inputs caused undefined behavior in the ProWizard loader due to
	 * missing checks on the sample offset and sample loop offset. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p40_invalid_sample");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_sample)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p40_invalid_sample2");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_sample2)");

	/* This input caused uninitialized reads in the ProWizard loader due to
	 * the input being a The Player 4.x "module" with 0 patterns. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p40_invalid_pattern_count");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_count)");

	xmp_free_context(opaque);
}
END_TEST
