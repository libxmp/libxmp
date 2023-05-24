#include "test.h"

/**
 * space traveller ][ by GDM is a CIA-timed module that is long enough
 * to get rescanned as VBlank and does not use low Fxx (which could
 * prevent the scan compare). The scan compare should prefer CIA timing
 * and revert back to the CIA scan.
 */

TEST(test_module_length_space_traveller)
{
	xmp_context opaque;
	struct xmp_module_info mi;
	struct xmp_frame_info fi;
	int ret, time = 0;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/p/space traveller 2.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &mi);
	xmp_get_frame_info(opaque, &fi);

	fail_unless(mi.mod->len == 70, "module length");
	fail_unless(fi.total_time == 700000, "estimated time");

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	while (xmp_play_frame(opaque) == 0) {
		xmp_get_frame_info(opaque, &fi);
		if (fi.loop_count > 0)
			break;

		time += fi.frame_time;
	}
	xmp_end_player(opaque);

	fail_unless(time / 1000 == 699982, "elapsed time");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
