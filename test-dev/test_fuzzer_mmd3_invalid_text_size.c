#include "test.h"

/* This input caused high RAM usage in the OctaMED loaders due to
 * reading the info text length as signed value and allowing negative lengths.
 */

TEST(test_fuzzer_mmd3_invalid_text_size)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd3_invalid_text_size.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
