#include "test.h"

/* This input caused hangs due to a missing EOF check in the
 * STMIK pattern loading loop.
 */

TEST(test_fuzzer_stx_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_stx_truncated.stx");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
