#include "test.h"

/* MED v2.00 and up support "compatibility tempos". These are intended
 * to be intercompatible with MOD speeds, but they actually set the tempo
 * rather than the speed. The speed should remain 6 for all tested MEDs.
 */

TEST(test_player_med_compat_tempo)
{
	static const char *check_files[] = {
		"data/med3_compat_tempo.med",
		"data/med4_compat_tempo.med",
		"data/mmd0_compat_tempo.med",
		"data/mmd2_compat_tempo.med",
		NULL
	};
	/* MED2 doesn't support compat tempos, but test the initial time factor. */
	static const char *time_factor_only[] = {
		"data/m/med2test.med",
		NULL
	};
	const char **file;

	xmp_context opaque;
	struct context_data *ctx;
	struct module_data *m;
	struct xmp_frame_info fi;
	int ret;

	opaque = xmp_create_context();
	fail_unless(opaque, "failed to create context");
	ctx = (struct context_data *)opaque;
	m = &ctx->m;

	for (file = check_files; *file != NULL; file++) {
		ret = xmp_load_module(opaque, *file);
		fail_unless(ret == 0, "failed to load module");
		fail_unless(m->time_factor == MED_TIME_FACTOR, "time factor mismatch");
		fail_unless(m->mod.spd == 6, "initial speed not 6");

		ret = xmp_start_player(opaque, XMP_MIN_SRATE, XMP_FORMAT_MONO);
		fail_unless(ret == 0, "failed to start player");
		xmp_set_player(opaque, XMP_PLAYER_INTERP, XMP_INTERP_NEAREST);

		while (1) {
			xmp_play_frame(opaque);
			xmp_get_frame_info(opaque, &fi);
			if (fi.loop_count >= 1)
				break;

			fail_unless(m->mod.spd == 6, "compat tempo changed speed");
		}
	}

	for (file = time_factor_only; *file != NULL; file++) {
		ret = xmp_load_module(opaque, *file);
		fail_unless(ret == 0, "failed to load module");
		fail_unless(m->time_factor == MED_TIME_FACTOR, "time factor mismatch");
		fail_unless(m->mod.spd == 6, "initial speed not 6");
	}

	xmp_free_context(opaque);
}
END_TEST
