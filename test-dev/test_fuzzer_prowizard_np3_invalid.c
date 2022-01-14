#include "test.h"


TEST(test_fuzzer_prowizard_np3_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the NoisePacker 3
	 * depacker due to a missing note bounds check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_np3_invalid_note");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused an out-of-bounds write in the NoisePacker 3
	 * depacker due to a missing pattern count bounds check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_np3_invalid_pattern_count.xz");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_pattern_count)");

	xmp_free_context(opaque);
}
END_TEST
