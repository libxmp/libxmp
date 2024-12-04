#include "test.h"

/**
 * This Octalyser CD81 module relies on being played in Octalyser
 * 4 channel mode or 6 channel mode, as there are duplicate E6x
 * effects in the last two channels. This information is NOT
 * stored in the module.
 *
 * As of writing this comment, libxmp supports this module by
 * enabling an inaccurate loop mode quirk for Octalyser MODs,
 * but this module could also be fixed by checking for its MD5.
 *
 * In addition to this, it relies on global loop target/count,
 * and found a bug in how pattern delay was being scanned.
 *
 * TODO: not clear why the scan and real time are >200ms apart.
 */

TEST(test_module_length_dammed_illusion)
{
	xmp_context opaque;
	struct xmp_module_info mi;
	struct xmp_frame_info fi;
	int ret, time = 0;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/p/dammed_illusion.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &mi);
	xmp_get_frame_info(opaque, &fi);

	fail_unless(mi.mod->len == 96, "module length");
	fail_unless(fi.total_time == 354200, "estimated time");

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	while (xmp_play_frame(opaque) == 0) {
		xmp_get_frame_info(opaque, &fi);
		if (fi.loop_count > 0)
			break;

		time += fi.frame_time;
	}
	xmp_end_player(opaque);

	fail_unless(time / 1000 == 354435, "elapsed time");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
