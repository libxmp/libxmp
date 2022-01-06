#include "test.h"

TEST(test_fuzzer_depack_arc_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused large allocations in the nomarch ARC/ArcFS LZW
	 * decoder due to having a large original length field.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arc_invalid_size");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_size)");

	/* This input caused out of bounds reads in the nomarch ARC depacker
	 * when attempting to find the first character of invalid strings.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arc_invalid_lzw");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_lzw)");

	/* This input caused leaks in the nomarch ARC depacker due to
	 * failing to free data after encountering an invalid in-stream
	 * LZW maximum bitwidth for method FF.
	 */
	ret = xmp_load_module(opaque, "data/f/depack_arc_invalid_width_FF");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_width_FF)");

	xmp_free_context(opaque);
}
END_TEST
