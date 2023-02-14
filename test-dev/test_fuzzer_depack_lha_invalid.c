#include "test.h"


TEST(test_fuzzer_depack_lha_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused out-of-bounds writes to the c_len table in the
	 * LHA depacker due to a missing check on the maximum table length. */
	ret = xmp_load_module(opaque, "data/f/depack_lha_invalid_clen.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_clen)");

	/* This input caused hangs and large file output from the LHA depacker
	 * due to characters in the Huffman tree with 0-bit encodings. */
	ret = xmp_load_module(opaque, "data/f/depack_lha_invalid_tree.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_tree)");

	/* This input caused infinite loops in the LHA depacker due to
	 * attempting to skip a file with a file size that libxmp loads as
	 * -53, negative the length of the LHA file header :( */
	ret = xmp_load_module(opaque, "data/f/depack_lha_invalid_size.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size)");

	/* This input caused signed underflows from the same negative length
	 * problem, this time in the level 1 header parser. */
	ret = xmp_load_module(opaque, "data/f/depack_lha_invalid_size2.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size2)");

	/* Uncompressed sizes >2G would result in the depacker code
	 * trying to allocate negative numbers. */
	ret = xmp_load_module(opaque, "data/f/depack_lha_invalid_size3.lha");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size3)");

	xmp_free_context(opaque);
}
END_TEST
