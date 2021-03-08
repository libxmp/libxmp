#include "test.h"

/* This input caused uninititalized reads in the MMD2/3 loader due to
 * not checking for an EOF after reading instrument names.
 */

TEST(test_fuzzer_mmd3_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd3_truncated.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
