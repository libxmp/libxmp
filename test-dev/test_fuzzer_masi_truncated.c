#include "test.h"

/* This input caused uninitialized reads in the MASI loader
 * due to a missing EOF check when checking the pattern count.
 */

TEST(test_fuzzer_masi_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_masi_truncated.psm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
