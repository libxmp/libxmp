#include "test.h"

TEST(test_player_scan)
{
	xmp_context opaque;
	struct xmp_frame_info info;
	int ret;

	opaque = xmp_create_context();
	fail_unless(opaque != NULL, "can't create context");

	ret = xmp_load_module(opaque, "data/ode2ptk.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_start_player(opaque, 44100, 0);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.total_time == 85472, "incorrect total time");

	/* This is a very long OctaMED module with 256 orders of 3200 rows,
	 * at BPM 28/LPB 1, speed 32. Currently the scan can't report the
	 * length accurately, but it should at least not reject this module
	 * and try to report a usable value. */
	ret = xmp_load_module(opaque, "data/longest.med");
	fail_unless(ret == 0, "can't load module (2)");

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.total_time == INT_MAX, "incorrect total time");

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
