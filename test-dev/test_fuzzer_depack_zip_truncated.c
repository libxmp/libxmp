#include "test.h"


TEST(test_fuzzer_depack_zip_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused a memory leak in kunzip inflate.c due to bad
	 * handling of the fixed Huffman tree pointer. Currently it just
	 * covers some failure cases of tinfl. */
	ret = xmp_load_module(opaque, "data/f/depack_zip_truncated2.zip");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated2)");

	/* This input caused an infinite loop in the ZIP depacker. */
	ret = xmp_load_module(opaque, "data/f/depack_zip_truncated3.zip");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated3)");

	/* This input caused memory leaks in the ZIP depacker. */
	ret = xmp_load_module(opaque, "data/f/depack_zip_truncated4.zip");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (truncated4)");

	xmp_free_context(opaque);
}
END_TEST
