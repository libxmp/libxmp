#include "test.h"

TEST(test_loader_med3_song)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_med3_song.data", "r");

	opaque = xmp_create_context();
	/* This format supports both module and song files. */
	ret = xmp_set_instrument_path(opaque, "data/m");
	fail_unless(ret == 0, "set instrument path");

	ret = xmp_load_module(opaque, "data/m/med3song.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	rewind(f);

	/* libxmp can load samples from the module directory too. */
	ret = xmp_set_instrument_path(opaque, "jgklfjdgk");
	fail_unless(ret == 0, "set instrument path (junk)");

	ret = xmp_load_module(opaque, "data/m/med3song.med");
	fail_unless(ret == 0, "module load (junk instrument path)");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded (junk instrument path)");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
