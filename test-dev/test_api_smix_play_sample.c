#include "../src/common.h"
#include "../src/mixer.h"
#include "../src/player.h"
#include "../src/virtual.h"
#include "test.h"

TEST(test_api_smix_play_sample)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	struct mixer_voice *vi;
	struct channel_data *xc;
	int voc, ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	ret = xmp_load_module(opaque, "data/mod.loving_is_easy.pp");
	fail_unless(ret == 0, "load module");

	xmp_start_smix(opaque, 1, 4);

	ret = xmp_smix_load_sample(opaque, 0, "data/blip.wav");
	fail_unless(ret == 0, "load sample 0");
        ret = xmp_smix_load_sample(opaque, 1, "data/buzz.wav");
	fail_unless(ret == 0, "load sample 1");
	ret = xmp_smix_load_sample(opaque, 2, "data/send.wav");
	fail_unless(ret == 0, "load sample 2");
	/* leave sample 3 unloaded */

	/* play sample before starting player */
	ret = xmp_smix_play_sample(opaque, 0, 60, 64, 0);
	fail_unless(ret == -XMP_ERROR_STATE, "invalid state");

	xmp_start_player(opaque, 44100, 0);
	xmp_play_frame(opaque);

	/* play invalid sample */
	ret = xmp_smix_play_sample(opaque, 4, 60, 64, 0);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid sample");

	ret = xmp_smix_play_sample(opaque, -1, 60, 64, 0);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid sample");

	/* play sample in invalid channel */
	ret = xmp_smix_play_sample(opaque, 0, 60, 64, 1);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel");

	ret = xmp_smix_play_sample(opaque, 0, 60, 64, -1);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel");

	ret = xmp_smix_play_sample(opaque, 0, 60, 64, 0);
	fail_unless(ret == 0, "play sample");
	xmp_play_frame(opaque);

	voc = map_channel(p, 4);
	fail_unless(voc >= 0, "virtual map");
	vi = &p->virt.voice_array[voc];

	fail_unless(vi->note - ctx->smix.xxi[0].sub[0].xpo == 60, "set note");
	fail_unless(vi->ins  ==  31, "set instrument");
	fail_unless(vi->vol / 16 == 64, "set volume");
	fail_unless(vi->pos0 ==  0, "sample position");

	ret = xmp_smix_play_sample(opaque, 1, 50, 40, 0);
	fail_unless(ret == 0, "play_sample");
	xmp_play_frame(opaque);

	fail_unless(vi->note - ctx->smix.xxi[1].sub[0].xpo == 50, "set note");
	fail_unless(vi->ins  ==  32, "set instrument");
	fail_unless(vi->vol / 16 == 40, "set volume");
	fail_unless(vi->pos0 ==  0, "sample position");

	/* key off, fade, and cut */
	xc = &p->xc_data[ctx->m.mod.chn];
	ret = xmp_smix_play_sample(opaque, 1, XMP_KEY_OFF, 0, 0);
	fail_unless(ret == 0, "play key off");
	xmp_play_frame(opaque);

	fail_unless(vi->flags & VOICE_RELEASE, "voice release");
	fail_unless(!TEST_NOTE(NOTE_FADEOUT), "no note fadeout");
	fail_unless(xc->period, "no note end");

	ret = xmp_smix_play_sample(opaque, 1, XMP_KEY_FADE, 0, 0);
	fail_unless(ret == 0, "play key fade");
	xmp_play_frame(opaque);

	fail_unless(TEST_NOTE(NOTE_FADEOUT), "note fadeout");
	fail_unless(xc->period, "no note end");

	ret = xmp_smix_play_sample(opaque, 1, XMP_KEY_CUT, 0, 0);
	fail_unless(ret == 0, "play key cut");
	xmp_play_frame(opaque);

	fail_unless(!xc->period, "note end");

	/* play stereo sample */
	ret = xmp_smix_play_sample(opaque, 2, 40, 55, 0);
	fail_unless(ret == 0, "play_sample");
	xmp_play_frame(opaque);

	fail_unless(vi->note - ctx->smix.xxi[2].sub[0].xpo == 40, "set note");
	fail_unless(vi->ins == 33, "set instrument");
	fail_unless(vi->vol / 16 == 55, "set volume");
	fail_unless(vi->pos0 == 0, "sample position");

	/* play unloaded sample */
	ret = xmp_smix_play_sample(opaque, 3, 60, 64, 0);
	xmp_play_frame(opaque);

	/* smix sample 2 is still playing */
	fail_unless(vi->ins == 33, "should still be 33");
	fail_unless(vi->smp == 33, "should still be 33");
	fail_unless(vi->pos0 != 0, "sample position should be non-zero");
	fail_unless(!TEST_NOTE(NOTE_SAMPLE_END), "sample should be active");

	xmp_smix_release_sample(opaque, 0);
	xmp_smix_release_sample(opaque, 1);
	xmp_smix_release_sample(opaque, 2);

	/* release should have cleared the channel to avoid crashes */
	fail_unless(vi->ins == 0, "should have been cleared");
	fail_unless(vi->smp == 0, "should have been cleared");
	fail_unless(vi->pos0 == 0, "should have been cleared");
	fail_unless(vi->sptr == NULL, "should have been cleared");
	xmp_play_frame(opaque);

	/* play released sample */
	ret = xmp_smix_play_sample(opaque, 0, 60, 64, 0);
	xmp_play_frame(opaque);

	xmp_end_player(opaque);
	xmp_end_smix(opaque);

	xmp_start_player(opaque, 44100, 0);

	/* Try to play sample after deinit */
	ret = xmp_smix_play_sample(opaque, 1, 60, 64, 0);
	fail_unless(ret == -XMP_ERROR_INVALID, "play sample after deinit");

	xmp_free_context(opaque);
}
END_TEST
