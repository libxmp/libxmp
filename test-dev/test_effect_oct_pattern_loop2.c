#include "test.h"
#include "../src/effects.h"

static int patternloop_Vals2[] = {
	0, 0, 0,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	1, 1, 1,
	2, 2, 2,
	3, 3, 3,
	4, 4, 4
};

/* Atari Octalyser seems to have the loop arguments as global
 * instead of channel separated. At least what I can see from
 * 8er-mod.
 *
 * The replay sources that I got my hands on, does not support E6x,
 * so I can not verify it.
 *
 * However, Dammed Illusion have the same E6x on multiple channels,
 * so this test will check if the correct behavior still happens. It
 * should just handle the effects as "one".
 */

TEST(test_effect_oct_pattern_loop2)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

 	create_simple_module(ctx, 2, 2);
	set_quirk(ctx, QUIRK_OCTALYSERLOOP, READ_EVENT_MOD);

	new_event(ctx, 0, 0, 0, 60, 1, 0, 0, 0, FX_SPEED, 3);
	new_event(ctx, 0, 1, 0, 0, 0, 0, FX_EXTENDED, 0x60, 0, 0);
	new_event(ctx, 0, 1, 1, 0, 0, 0, FX_EXTENDED, 0x60, 0, 0);
	new_event(ctx, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0);
	new_event(ctx, 0, 3, 0, 0, 0, 0, FX_EXTENDED, 0x64, 0, 0);
	new_event(ctx, 0, 3, 1, 0, 0, 0, FX_EXTENDED, 0x64, 0, 0);
	new_event(ctx, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0);

	xmp_start_player(opaque, 44100, 0);

	for (i = 0; i < 14 * 3; i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		fail_unless(info.row == patternloop_Vals2[i], "row set error");
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
