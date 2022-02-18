#include "test.h"


TEST(test_fuzzer_depack_gz_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused hangs due to missing error checks in the
	 * gzip depacker when skipping the name and comment fields. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_name.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_name)");

	/* This input is missing the gzip footer. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_footer.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_footer)");

	/* This input has the wrong CRC-32 stored in the footer. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_crc.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_crc)");

	/* This input has the wrong length stored in the footer. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_length.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_length)");

	/* This input contains an invalid tree that tinfl reads zero-length
	 * codes from until it runs out of memory. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_tree.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_tree)");

	/* Same as invalid_tree, but it affected a different code branch. */
	ret = xmp_load_module(opaque, "data/f/depack_gz_invalid_tree2.gz");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_tree2)");

	xmp_free_context(opaque);
}
END_TEST
