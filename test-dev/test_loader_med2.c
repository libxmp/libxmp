#include "test.h"

TEST(test_loader_med2)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_med2.data", "r");

	opaque = xmp_create_context();
	/* This format only supports song files. */
	ret = xmp_set_instrument_path(opaque, "data/m");
	fail_unless(ret == 0, "set instrument path");

	ret = xmp_load_module(opaque, "data/m/med2test.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
