#include "test.h"

/* This input caused hangs and high RAM consumption in the IT loader
 * due to allocating large buffers for invalid compressed samples.
 */

TEST(test_fuzzer_it_invalid_compressed)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_it_invalid_compressed.it");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
