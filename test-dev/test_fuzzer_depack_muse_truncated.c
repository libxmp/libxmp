#include "test.h"


TEST(test_fuzzer_depack_muse_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the kunzip inflate
	 * implementation due to indexing an array by EOF without a check.
	 * It partly cover tinfl failure cases, too. */
	ret = xmp_load_module(opaque, "data/f/depack_muse_truncated.j2b");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated)");   /* -XMP_ERROR_DEPACK */

	/* This input caused an endless loop in the kunzip inflate
	 * implementation due to missing EOF checks. It partly covers
	 * tinfl failure cases, too. */
	ret = xmp_load_module(opaque, "data/f/depack_muse_truncated2.j2b");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated2)");  /* -XMP_ERROR_DEPACK */

	xmp_free_context(opaque);
}
END_TEST
