#include "test.h"

/* NOTE: libxmp playback gets a few odd things about LIQ modules wrong here:
 *
 * ins w/ no note after cut: should not play anything (libxmp sets volume).
 * note w/ no ins after cut: should play using previous sample, as FT2/IT
 *   (libxmp ignores; FT2 event handler fixes this).
 *
 * This format *probably* doesn't need its own read event implementation yet.
 */

TEST(test_loader_liq_smpl_setpan)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_liq_smpl_setpan.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/liq_smpl_setpan.liq");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
