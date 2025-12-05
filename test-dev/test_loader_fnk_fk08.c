#include "test.h"

/* FunkTracker DOS module with fixed 669-like hard panning. */

TEST(test_loader_fnk_fk08)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_fnk_fk08.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/fnk_fk08.fnk");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
