#include "test.h"
#include "../src/effects.h"

static const double times[] =
{
	40.0,
	80.0,
	400.0,
	720.0,
	2320.0,
	3920.0,
	3940.0,
};

TEST(test_effect_med_patt_delay)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	size_t i;
	double time;
	int ret;
	char buf[80];

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	create_simple_module(ctx, 2, 2);
	new_event(ctx, 0, 0, 0, 0, 0, 0, FX_PATT_DELAY, 0x01, 0,             0x00);
	new_event(ctx, 0, 1, 0, 0, 0, 0, 0,             0x00, FX_PATT_DELAY, 0x01);
	new_event(ctx, 0, 2, 0, 0, 0, 0, FX_PATT_DELAY, 0x0f, 0,             0x00);
	new_event(ctx, 0, 3, 0, 0, 0, 0, 0,             0x00, FX_PATT_DELAY, 0x0f);
	new_event(ctx, 0, 4, 0, 0, 0, 0, FX_PATT_DELAY, 0x4f, 0,             0x00);
	new_event(ctx, 0, 5, 0, 0, 0, 0, 0,             0x00, FX_PATT_DELAY, 0x4f);
	new_event(ctx, 0, 6, 0, 0, 0, 0, 0,             0x00, FX_BREAK,      0x00);

	libxmp_free_scan(ctx);
	ctx->m.mod.len = 1;
	ctx->m.mod.spd = 1;
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	time = times[ARRAY_SIZE(times) - 1];
	snprintf(buf, sizeof(buf), "scan duration mismatch: %.02f != %.02f",
		p->scan[0].time, time);
	fail_unless(p->scan[0].time == time, buf);

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);

	for (i = 0; i < ARRAY_SIZE(times); i++) {
		/* Want the time from the end of the row--play
		 * until the next row is reached, then use the
		 * previous time. */
		do {
			time = p->current_time;
			ret = xmp_play_frame(opaque);
		} while(ret == 0 && p->row == (int)i);

		snprintf(buf, sizeof(buf),
			"current time @ %d mismatch: %.02f != %.02f",
			(int)i, time, times[i]);
		fail_unless(time == times[i], buf);
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
