#include "test.h"

/* This input caused memory corruption in the MMD2/3 loader due to
 * junk data at an invalid block array table offset getting
 * interpreted as valid block offsets.
 */

TEST(test_fuzzer_mmd3_invalid_blockarr)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd3_invalid_blockarr.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
