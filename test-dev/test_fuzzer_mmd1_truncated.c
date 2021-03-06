#include "test.h"

/* This input caused uninititalized reads in the MMD0/1 loader due to
 * not checking for an EOF after reading instrument names.
 */

TEST(test_fuzzer_mmd1_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd1_truncated.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
