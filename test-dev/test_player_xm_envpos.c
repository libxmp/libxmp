#include "test.h"
#include "../src/effects.h"

/*
If the envelope position is set outside the loop via Lxx effect,
Fasttracker 2 will ignore the loop.
*/
TEST(test_player_xm_envpos)
{
	xmp_context opaque;
	struct xmp_frame_info fi;
	struct context_data *ctx;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

 	create_simple_module(ctx, 2, 2);

	/* Envelope from Ebony Owl Netsuke.xm, first instrument */
	set_instrument_envelope(ctx, 0, 0, 0, 59);
	set_instrument_envelope(ctx, 0, 1, 1, 30);
	set_instrument_envelope(ctx, 0, 2, 2, 1);
	set_instrument_envelope(ctx, 0, 3, 9, 0);
	set_instrument_envelope(ctx, 0, 4, 34, 62);
	set_instrument_envelope(ctx, 0, 5, 35, 15);
	set_instrument_envelope(ctx, 0, 6, 36, 0);
	set_instrument_envelope(ctx, 0, 7, 93, 0);
	set_instrument_envelope(ctx, 0, 8, 94, 59);
	set_instrument_envelope(ctx, 0, 9, 99, 31);
	set_instrument_envelope(ctx, 0, 10, 102, 19);
	set_instrument_envelope(ctx, 0, 11, 106, 11);
	set_instrument_envelope_loop(ctx, 0, 0, 0);
	set_instrument_envelope_sus(ctx, 0, 3);

	new_event(ctx, 0, 0, 0, 60, 1, 0, FX_ENVPOS, 34, 0, 0);
	set_quirk(ctx, QUIRKS_FT2 | QUIRK_FT2ENV, READ_EVENT_FT2);

	xmp_start_player(opaque, 44100, 0);

	/* Frame 0 */
	xmp_play_frame(opaque);
	xmp_get_frame_info(opaque, &fi);
	fail_unless(fi.channel_info[0].volume == 15, "wrong volume at frame 0");

	/* Frame 1 */
	xmp_play_frame(opaque);
	xmp_get_frame_info(opaque, &fi);
	fail_unless(fi.channel_info[0].volume == 0, "wrong volume at frame 1");

	/* Frame 2 */
	xmp_play_frame(opaque);
	xmp_get_frame_info(opaque, &fi);
	fail_unless(fi.channel_info[0].volume == 0, "wrong volume at frame 2");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
