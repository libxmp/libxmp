#include "test.h"

/* Futureproofing test for MMD0: if odd loop values are
 * present in these modules and loaded, they should not be
 * applied. OctaMED Soundstudio only uses them in mixing mode;
 * MED Soundstudio 2 always uses them, but only saves MMD3s.
 */

TEST(test_loader_mmd0_longrepeat)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_mmd0_longrepeat.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/mmd0_longrepeat.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
