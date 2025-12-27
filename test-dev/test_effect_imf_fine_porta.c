#include "test.h"
#include "../src/effects.h"

/* Imago Orpheus IMF fine portamento effects Kxx/Lxx share
 * memory with Ixx (porta up) and Jxx (porta down) and have
 * 1/16th the precision of normal portamento (rather than
 * XM X1y/X2y 1/4th precision).
 */

#define FINE 0.0625

static double deltas[] = {
	 0.0,		0.0, 0.0, 0.0,
	-FINE * 0x0f,	0.0, 0.0, 0.0,		/* K0F */
	-FINE * 0x71,	0.0, 0.0, 0.0,		/* K71 */
	 FINE * 0x69,	0.0, 0.0, 0.0,		/* L69 */
	 FINE * 0x17,	0.0, 0.0, 0.0,		/* L17 */
	 0.0,		-0x23, -0x23, -0x23,	/* I23 */
	 FINE * 0x23,	0.0, 0.0, 0.0,		/* L00 */
	-FINE * 0x23,	0.0, 0.0, 0.0,		/* K00 */
	 0.0,		0x23, 0x23, 0x23,	/* J00 */
	 0.0,		0x14, 0x14, 0x14,	/* J14 */
	-FINE * 0x14,	0.0, 0.0, 0.0,		/* K00 */
	 0.0,		-0x14, -0x14, -0x14,	/* I00 */
	 FINE * 0x14,	0.0, 0.0, 0.0,		/* L00 */
	-FINE * 0x1e,	0.0, 0.0, 0.0,		/* K1E */
	 0.0,		0x1e, 0x1e, 0x1e,	/* J00 */
	 FINE * 0x1e,	0.0, 0.0, 0.0,		/* L00 */
	 0.0,		-0x1e, -0x1e, -0x1e,	/* I00 */
	 FINE * 0x31,	0.0, 0.0, 0.0,		/* L31 */
	 0.0,		-0x31, -0x31, -0x31,	/* I00 */
	 0.0,		0x31, 0x31, 0x31,	/* J00 */
	-FINE * 0x31,	0.0, 0.0, 0.0,		/* K00 */
};

TEST(test_effect_imf_fine_porta)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	char mesg[80];
	double d;
	int next, prev;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	create_simple_module(ctx, 2, 2);

	/* IMF fine up/down */
	new_event(ctx, 0,  0, 0, 49, 1, 0, FX_SPEED, 0x04, 0, 0);
	new_event(ctx, 0,  1, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x0f, 0, 0);
	new_event(ctx, 0,  2, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x71, 0, 0);
	new_event(ctx, 0,  3, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x69, 0, 0);
	new_event(ctx, 0,  4, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x17, 0, 0);

	/* Shared memory */
	new_event(ctx, 0,  5, 0,  0, 0, 0, FX_PORTA_UP, 0x23, 0, 0);
	new_event(ctx, 0,  6, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x00, 0, 0);
	new_event(ctx, 0,  7, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x00, 0, 0);
	new_event(ctx, 0,  8, 0,  0, 0, 0, FX_PORTA_DN, 0x00, 0, 0);

	new_event(ctx, 0,  9, 0,  0, 0, 0, FX_PORTA_DN, 0x14, 0, 0);
	new_event(ctx, 0, 10, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x00, 0, 0);
	new_event(ctx, 0, 11, 0,  0, 0, 0, FX_PORTA_UP, 0x00, 0, 0);
	new_event(ctx, 0, 12, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x00, 0, 0);

	new_event(ctx, 0, 13, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x1e, 0, 0);
	new_event(ctx, 0, 14, 0,  0, 0, 0, FX_PORTA_DN, 0x00, 0, 0);
	new_event(ctx, 0, 15, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x00, 0, 0);
	new_event(ctx, 0, 16, 0,  0, 0, 0, FX_PORTA_UP, 0x00, 0, 0);

	new_event(ctx, 0, 17, 0,  0, 0, 0, FX_IMF_FPORTA_DN, 0x31, 0, 0);
	new_event(ctx, 0, 18, 0,  0, 0, 0, FX_PORTA_UP, 0x00, 0, 0);
	new_event(ctx, 0, 19, 0,  0, 0, 0, FX_PORTA_DN, 0x00, 0, 0);
	new_event(ctx, 0, 20, 0,  0, 0, 0, FX_IMF_FPORTA_UP, 0x00, 0, 0);

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);

	xmp_play_frame(opaque);
	xmp_get_frame_info(opaque, &info);
	prev = info.channel_info[0].period;

	for (i = 1; i < (int)ARRAY_SIZE(deltas); i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		next = info.channel_info[0].period;

		d = (double)(next - prev) / 4096.0 - deltas[i];
		snprintf(mesg, sizeof(mesg), "%d: delta error: %f != %f",
			 i, (double)(next - prev) / 4096.0, deltas[i]);
		fail_unless(d >= -0.01 && d <= 0.01, mesg);
		prev = next;
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
