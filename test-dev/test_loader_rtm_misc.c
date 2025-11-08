#include "test.h"

/* Tests for misc. RTM properties fixed around the same time:
 * 1) Patterns can have 999 rows
 * 2) Fixed loading of effects 8/a/d/e/f/k
 * 3) Stripping of fine effects interpretations of 1/2/A
 *    (TODO should be ignored at runtime instead)
 * 4) Instrument mute samples flag
 * 5) Sample default panning (and lack thereof)
 * 6) Sample base volume (equivalent to S3M/IT global volume)
 *
 * This module doesn't actually play properly due to the
 * coverage for effects 1/2/e/f and A/d relying on separate
 * effects memory, which libxmp doesn't currently support.
 */

TEST(test_loader_rtm_misc)
{
	xmp_context opaque;
	struct xmp_module_info info;
	struct xmp_frame_info fi;
	FILE *f;
	int ret;

	f = fopen("data/format_rtm_misc.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/rtm_misc.rtm");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	/* Also test module length for the 999 row pattern. */
	xmp_get_frame_info(opaque, &fi);
	ret = (fi.total_time - 33075);
	fail_unless(ret >= -1 && ret <= 1, "wrong scan duration");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
