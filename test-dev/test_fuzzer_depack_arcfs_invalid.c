#include "test.h"

TEST(test_fuzzer_depack_arcfs_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused invalid shifts in the ArcFS depacker due to no
	 * bounds check on the compression bits value read from the format
	 * header.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arcfs_invalid_width_8");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_width_8)");

	/* This input originally caused leaks in the nomarch ArcFS depacker
	 * but now makes sure the header entries_length check works.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arcfs_invalid_entries_length");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_entries_length)");

	/* This input has a very large uncompressed size, make sure it gets
	 * ignored.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arcfs_invalid_size");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size)");

	/* This input has a very large compressed size, make sure it gets
	 * ignored.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arcfs_invalid_size_compr");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size_compr)");

	xmp_free_context(opaque);
}
END_TEST
