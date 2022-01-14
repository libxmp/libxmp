#include "test.h"


TEST(test_fuzzer_prowizard_np2_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused out-of-bounds writes in the NoisePacker 2
	 * depacker due to not validating the number of patterns. */
	ret = xmp_load_module(opaque, "data/f/prowizard_np2_invalid_pattern_count.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_count)");

	xmp_free_context(opaque);
}
END_TEST
