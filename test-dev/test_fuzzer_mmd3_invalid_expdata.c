#include "test.h"

/* This input caused high RAM usage in the OctaMED loaders due to
 * reading the info text length as signed value and allowing negative lengths.
 * Its expdata is truncated, which now gets caught instead--see invalid_mmdinfo
 * for the updated test module for this.
 */

TEST(test_fuzzer_mmd3_invalid_expdata)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd3_invalid_expdata.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
