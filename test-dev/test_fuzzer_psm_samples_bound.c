#include "test.h"

/* This input crashed the PSM loader due to containing a samples
 * count over 64, which overflowed the sample offsets array.
 */

TEST(test_fuzzer_psm_samples_bound)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_psm_samples_bound.psm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
