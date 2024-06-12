#include "test.h"

/* Data immediately prior to and after the loop should not
 * affect the playback of the loop when using interpolation.
 * The loop of the sample in this module should be completely
 * silent.
 *
 * Previously, libxmp played a buzz instead due to the sample
 * prior to the loop not being fixed with spline interpolation.
 * Similar behavior can be found in Modplug Tracker 1.16.
 */

TEST(test_mixer_interpolation_loop)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct mixer_data *s;
	int i, j, ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	s = &ctx->s;

	ret = xmp_load_module(opaque, "data/interpolation_loop.it");
	fail_unless(ret == 0, "load error");

	xmp_start_player(opaque, 8000, XMP_FORMAT_MONO);
	xmp_set_player(opaque, XMP_PLAYER_INTERP, XMP_INTERP_SPLINE);

	/* First frame is the only one that should contain data. */
	compare_mixer_samples_ext(ctx, "data/interpolation_loop.data", 0, 1);

	/* Further frames should be silent. */
	for (i = 0; i < 10; i++) {
		xmp_play_frame(opaque);
		for (j = 0; j < s->ticksize; j++)
			fail_unless(s->buf32[j] == 0, "mixing error");
	}

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
