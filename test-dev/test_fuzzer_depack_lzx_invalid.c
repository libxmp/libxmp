#include "test.h"


TEST(test_fuzzer_depack_lzx_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the XAD LZW depacker
	 * by not bounds checking the second code read for pretree value 19,
	 * resulting in indexing a modulo table by -2. */
	ret = xmp_load_module(opaque, "data/f/depack_lzx_invalid.lzx");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
