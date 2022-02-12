#include "test.h"


TEST(test_fuzzer_depack_bz2_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* These two inputs caused different invalid stack unwinds
	 * when calling longjmp at EOF. */
	ret = xmp_load_module(opaque, "data/f/depack_bz2_truncated.bz2");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated)");

	ret = xmp_load_module(opaque, "data/f/depack_bz2_truncated2.bz2");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated2)");

	xmp_free_context(opaque);
}
END_TEST
