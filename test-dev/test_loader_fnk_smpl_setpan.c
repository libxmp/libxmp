#include "test.h"

/* FunkTracker has some very strange behavioral quirks. This module currently
 * plays incorrectly:
 *
 * - Instrument-only "slot" (note 0x3e) sets default panning, but
 *   "reload sample attrs slot" (note 0x3d) does not. libxmp plays row 28 wrong.
 */

TEST(test_loader_fnk_smpl_setpan)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_fnk_smpl_setpan.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/fnk_smpl_setpan.fnk");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
