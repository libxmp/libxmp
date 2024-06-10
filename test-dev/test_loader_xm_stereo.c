#include "test.h"

/* Test loading 8-bit and 16-bit stereo XM samples (delta coded). */

TEST(test_loader_xm_stereo)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/stereo_xm.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/stereo.xm");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
