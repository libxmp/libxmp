#include "test.h"

TEST(test_api_prev_position)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	int state, ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	state = xmp_get_player(opaque, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	ret = xmp_prev_position(opaque);
	fail_unless(ret == -XMP_ERROR_STATE, "state check error");

	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0);
	set_order(ctx, 1, 1);
	set_order(ctx, 2, 0);

	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	xmp_start_player(opaque, 44100, 0);
	fail_unless(p->ord == 0, "didn't start at pattern 0");

	ret = xmp_prev_position(opaque);
	fail_unless(ret == 0, "prev position error");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 0, "not in position 0");

	xmp_set_position(opaque, 2);
	xmp_play_frame(opaque);
	fail_unless(p->ord == 2, "didn't set position 2");

	ret = xmp_prev_position(opaque);
	fail_unless(ret == 1, "prev position error");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 1, "didn't change to prev position");

	/* xmp_prev_position should not restart a stopped module. */
	xmp_stop_module(opaque);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "didn't stop module");
	ret = xmp_prev_position(opaque);
	fail_unless(ret == 0, "not in position 0");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "module should still be stopped");

	/* xmp_prev_position should not be able to seek backwards from restart. */
	xmp_restart_module(opaque);
	ret = xmp_prev_position(opaque);
	fail_unless(ret == 0, "didn't change to position 0");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 0, "not in position 0");

	/* xmp_prev_position should not be confused by a skip marker
	 * at position 0. */
	xmp_end_player(opaque);

	libxmp_free_scan(ctx);
	set_order(ctx, 0, XMP_MARK_SKIP);
	set_order(ctx, 1, 0);
	set_quirk(ctx, QUIRK_MARKER, READ_EVENT_IT);
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	xmp_start_player(opaque, 44100, 0); /* Skip marker position */
	xmp_play_frame(opaque);
	fail_unless(p->ord == 1, "didn't start at pattern 1");
	ret = xmp_prev_position(opaque);
	fail_unless(ret == 0, "prev position error");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 1, "not in position 1");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
