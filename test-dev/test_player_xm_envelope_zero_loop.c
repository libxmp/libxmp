#include "test.h"

/* Fasttracker 2 increments the envelope position before checking for
 * the end of the envelope loop, and only loops if the envelope position
 * is exactly at the envelope end. This causes it to entirely skip loops
 * where the start and end points are the same.
 */
TEST(test_player_xm_envelope_zero_loop)
{
	xmp_context opaque;
	struct xmp_frame_info fi;
	struct context_data *ctx;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	create_simple_module(ctx, 2, 2);

	set_instrument_envelope(ctx, 0, 0, 0, 64);
	set_instrument_envelope(ctx, 0, 1, 16, 0);
	set_instrument_envelope_loop(ctx, 0, 0, 0);

	new_event(ctx, 0, 0, 0, 60, 1, 0, 0, 0, 0, 0);
	set_quirk(ctx, QUIRKS_FT2 | QUIRK_FT2ENV, READ_EVENT_FT2);

	xmp_start_player(opaque, 44100, 0);

	for (i = 0; i < 16; i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &fi);

		fail_unless(fi.channel_info[0].volume == 64 - (i * 4), "wrong volume");
	}

	for (i = 0; i < 16; i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &fi);

		fail_unless(fi.channel_info[0].volume == 0, "volume not zero");
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
