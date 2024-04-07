#include "test.h"

TEST(test_module_length_klisje_paa_klisje)
{
	xmp_context opaque;
	struct xmp_module_info mi;
	struct xmp_frame_info fi;
	int ret, time = 0;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/p/klisje_paa_klisje.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &mi);
	xmp_get_frame_info(opaque, &fi);

	fail_unless(mi.mod->len == 93, "module length");
	fail_unless(fi.total_time == 637580, "estimated time");

	xmp_start_player(opaque, 8000, 0);
	while (xmp_play_frame(opaque) == 0) {
		xmp_get_frame_info(opaque, &fi);
		if (fi.loop_count > 0)
			break;

		time += fi.frame_time;
	}
	xmp_end_player(opaque);

	fail_unless(time / 1000 == fi.total_time, "elapsed time");

	xmp_release_module(opaque);

	/* Should also work */
	ret = xmp_set_player(opaque, XMP_PLAYER_HASH_TYPE, XMP_HASH_FASTEST);
	fail_unless(ret == 0, "set fast hash");
	ret = xmp_load_module(opaque, "data/p/klisje_paa_klisje.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &mi);
	xmp_get_frame_info(opaque, &fi);

	fail_unless(mi.mod->len == 93, "module length");
	fail_unless(fi.total_time == 637580, "estimated time");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
