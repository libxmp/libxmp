#include "test.h"
#include "../src/effects.h"

#include <math.h>

TEST(test_api_set_tempo_factor)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	struct module_data *m;
	struct xmp_frame_info info, prev;
	int state, ret, i;
	double factor;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;
	m = &ctx->m;

	state = xmp_get_player(opaque, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	create_simple_module(ctx, 1, 1);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0);

	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	/* This function relies on values initialized by xmp_start_player. */
	ret = xmp_set_tempo_factor(opaque, 1.0);
	fail_unless(ret == -XMP_ERROR_STATE, "should fail if not already playing");

	xmp_start_player(opaque, 44100, 0);
	fail_unless(p->ord == 0, "didn't start at pattern 0");

	ret = xmp_set_tempo_factor(opaque, 1.0);
	fail_unless(ret == 0, "should set to 1.0");

	/* Test xmp_set_tempo_factor's interactions with the current
	 * playback time. Play a few frames so the time is non-zero. */
	for (i = 0; i < 3; i++) {
		xmp_play_frame(opaque);
	}
	xmp_get_frame_info(opaque, &prev);
	ret = xmp_set_tempo_factor(opaque, 2.0);
	fail_unless(ret == 0, "should set to 2.0");
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.time == prev.time, "time should be the same prior to rescan");
	fail_unless(info.total_time == prev.total_time, "total time should be same prior to rescan");
	fail_unless(info.frame_time == prev.frame_time, "frame time should be same prior to rescan");
	xmp_play_frame(opaque);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.total_time == prev.total_time, "total time should be same prior to rescan (2)");
	fail_unless(info.frame_time == prev.frame_time, "frame time should be same prior to rescan (2)");
	prev = info;
	xmp_scan_module(opaque);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.time / 2 == prev.time, "time should be double after rescan");
	fail_unless(info.total_time / 2 == prev.total_time, "total time should be double after rescan");
	fail_unless(info.frame_time / 2== prev.frame_time, "frame time should be double after rescan");
	ret = xmp_set_tempo_factor(opaque, 0.5);
	fail_unless(ret == 0, "should set to 0.5");
	xmp_scan_module(opaque);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.time == prev.time / 2, "time should be half after rescan");
	fail_unless(info.total_time == prev.total_time / 2, "total time should be half after rescan");
	fail_unless(info.frame_time == prev.frame_time / 2, "frame time should be half after rescan");

	ret = xmp_set_tempo_factor(opaque, 0.0);
	fail_unless(ret == -1, "didn't fail to set tempo factor to 0.0");
	ret = xmp_set_tempo_factor(opaque, 1000.0);
	fail_unless(ret == -1, "didn't fail to set tempo factor to extreme value");
	ret = xmp_set_tempo_factor(opaque, -10.0);
	fail_unless(ret == -1, "didn't fail to set tempo factor to negative value");
#ifdef INFINITY
	ret = xmp_set_tempo_factor(opaque, INFINITY);
	fail_unless(ret == -1, "didn't fail to set tempo factor to infinity");
	ret = xmp_set_tempo_factor(opaque, -INFINITY);
	fail_unless(ret == -1, "didn't fail to set tempo factor to negative infinity");
#endif
#ifdef NAN
	ret = xmp_set_tempo_factor(opaque, NAN);
	fail_unless(ret == -1, "didn't fail to set tempo factor to NaN");
	ret = xmp_set_tempo_factor(opaque, -NAN);
	fail_unless(ret == -1, "didn't fail to set tempo factor to -NaN");
#endif

	/* Set oscillating BPMs to guarantee correct function at extremes. */
	libxmp_free_scan(ctx);
	new_event(ctx, 0, 0, 0, 0, 0, 0, FX_S3M_BPM, 0x21, FX_SPEED, 1);
	new_event(ctx, 0, 1, 0, 0, 0, 0, FX_S3M_BPM, 0xff, FX_BREAK, 0);
	m->mod.bpm = 255; /* Initial BPM */
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);
	xmp_restart_module(opaque);

	/* It should always work with reasonable tempo factors. */
	for (factor = 0.1; factor <= 2.0; factor += 0.1) {
		/* bpm = 255 */
		ret = xmp_set_tempo_factor(opaque, factor);
		fail_unless(ret == 0, "failed to set tempo factor (0)");
		xmp_play_frame(opaque);
		fail_unless(p->row == 0, "didn't play frame (0)");
		/* bpm = 33 */
		ret = xmp_set_tempo_factor(opaque, factor);
		fail_unless(ret == 0, "failed to set tempo factor (1)");
		xmp_play_frame(opaque);
		fail_unless(p->row == 1, "didn't play frame (1)");
	}

	/* Anything is fine here, as long as the mixer doesn't crash. */
	for (; factor <= 128.0; factor *= 2.0) {
		/* bpm = 255 */
		xmp_set_tempo_factor(opaque, factor);
		xmp_play_frame(opaque);
		fail_unless(p->row == 0, "didn't play frame (0)");
		/* bpm = 33 */
		xmp_set_tempo_factor(opaque, factor);
		xmp_play_frame(opaque);
		fail_unless(p->row == 1, "didn't play frame (1)");
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
