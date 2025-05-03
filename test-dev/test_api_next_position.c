#include "test.h"

TEST(test_api_next_position)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	int state, ret;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	state = xmp_get_player(opaque, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	ret = xmp_next_position(opaque);
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

	ret = xmp_next_position(opaque);
	fail_unless(ret == 1, "next position error");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 1, "didn't change to next position");

	xmp_set_position(opaque, 2);
	xmp_play_frame(opaque);
	fail_unless(p->ord == 2, "didn't set position 2");

	ret = xmp_next_position(opaque);
	fail_unless(ret == 2, "not in position 2");

	/* xmp_next_position should restart a stopped module. */
	xmp_stop_module(opaque);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "didn't stop module");
	ret = xmp_next_position(opaque);
	fail_unless(ret == -1, "didn't change to position -1");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 0, "not in position 0");

	/* xmp_next_position should not advance a restarting module. */
	xmp_restart_module(opaque);
	ret = xmp_next_position(opaque);
	fail_unless(ret == -1, "not at position -1");
	ret = xmp_next_position(opaque);
	fail_unless(ret == -1, "not at position -1");
	ret = xmp_next_position(opaque);
	fail_unless(ret == -1, "not at position -1");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 0, "not in position 0");

	/* xmp_next_position should not be confused by a module
	 * with 256 positions. */
	xmp_end_player(opaque);

	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0);
	for (i = 1; i < 256; i++)
		set_order(ctx, i, XMP_MARK_SKIP);
	set_quirk(ctx, QUIRK_MARKER, READ_EVENT_IT);
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	xmp_start_player(opaque, 44100, 0);
	fail_unless(p->ord == 0, "didn't start at pattern 0");
	ret = xmp_next_position(opaque);
	fail_unless(ret == 255, "next position error");
	xmp_play_frame(opaque);
	fail_unless(p->ord == 0, "not in position 0");
	ret = xmp_next_position(opaque);
	fail_unless(ret == 255, "next position error");
	ret = xmp_next_position(opaque);
	fail_unless(ret == 255, "not in position 255");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
