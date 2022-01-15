#include "test.h"


TEST(test_fuzzer_depack_compress_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused UMRs in the uncompress depacker due to containing
	 * an invalid maxbits value. */
	ret = xmp_load_module(opaque, "data/f/depack_compress_invalid_maxbits.Z");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
