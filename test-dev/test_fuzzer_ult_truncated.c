#include "test.h"

/* This input caused uninitialized reads in the ULT loader due to
 * not checking the return value of hio_read.
 */

TEST(test_fuzzer_ult_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_ult_truncated.ult");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
