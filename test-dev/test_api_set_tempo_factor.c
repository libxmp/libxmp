#include "test.h"
#include "../src/effects.h"

#include <math.h>

TEST(test_api_set_tempo_factor)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	struct module_data *m;
	int state, ret;
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

	new_event(ctx, 0, 0, 0, 0, 0, 0, FX_S3M_BPM, 0x21, FX_SPEED, 1);
	new_event(ctx, 0, 1, 0, 0, 0, 0, FX_S3M_BPM, 0xff, FX_BREAK, 0);
	m->mod.bpm = 255; /* Initial BPM */

	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	/* This function relies on values initialized by xmp_start_player. */
	ret = xmp_set_tempo_factor(opaque, 1.0);
	fail_unless(ret == -XMP_ERROR_STATE, "should fail if not already playing");

	xmp_start_player(opaque, 44100, 0);
	fail_unless(p->ord == 0, "didn't start at pattern 0");

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
