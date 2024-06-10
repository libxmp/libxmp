#include "test.h"

/* Test loading 8-bit and 16-bit stereo OctaMED samples. */

TEST(test_loader_mmd3_stereo)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/stereo_med.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/stereo.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
