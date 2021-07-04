#include "test.h"

/* This input caused stack corruption in the MMD0/1 test function
 * due to large uint32 values (in this case, EOF) being interpreted
 * as a negative offset in libxmp_read_title.
 */

TEST(test_fuzzer_mmd0_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd0_truncated.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
