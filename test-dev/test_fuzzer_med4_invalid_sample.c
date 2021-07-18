#include "test.h"

/* Inputs that may cause issues in the sample scanning loop in the MED4 loader.
 */

TEST(test_fuzzer_med4_invalid_sample)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample.med");
	fail_unless(ret == 0, "module load");

	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample2.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
