#include "test.h"

/* This input caused uninitialized memory reads in the MDL loader
 * due to not checking the hio_read return value for a truncated
 * instrument name.
 */

TEST(test_fuzzer_mdl_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mdl_truncated.mdl");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
