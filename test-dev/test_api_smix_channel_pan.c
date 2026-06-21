#include "test.h"
#include "../src/mixer.h"
#include "../src/virtual.h"

TEST(test_api_smix_channel_pan)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	struct mixer_voice *vi;
	int voc, ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	ret = xmp_load_module(opaque, "data/mod.loving_is_easy.pp");
	fail_unless(ret == 0, "load module");

	/* Try to set pan before initializing */
	ret = xmp_smix_channel_pan(opaque, 0, 0x55);
	fail_unless(ret == -XMP_ERROR_STATE, "set pan before init");

	xmp_start_smix(opaque, 1, 2);

	/* Try to set before starting player */
	ret = xmp_smix_channel_pan(opaque, 0, 0x66);
	fail_unless(ret == -XMP_ERROR_STATE, "set pan before start player");

	xmp_start_player(opaque, 44100, 0);

	/* set mix to 100% pan separation */
	xmp_set_player(opaque, XMP_PLAYER_MIX, 100);

	xmp_play_frame(opaque);

	ret = xmp_smix_play_instrument(opaque, 2, 60, 64, 0);
	fail_unless(ret == 0, "play_instrument");
	xmp_play_frame(opaque);

	voc = map_channel(p, 4);
	fail_unless(voc >= 0, "virtual map");
	vi = &p->virt.voice_array[voc];

	ret = xmp_smix_channel_pan(opaque, 2, 0x00);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel");

	ret = xmp_smix_channel_pan(opaque, -1, 0xaa);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid channel");

	ret = xmp_smix_channel_pan(opaque, 0, -1);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid pan");

	ret = xmp_smix_channel_pan(opaque, 0, 256);
	fail_unless(ret == -XMP_ERROR_INVALID, "invalid pan");

	xmp_smix_channel_pan(opaque, 0, 0x00);
	xmp_play_frame(opaque);
	fail_unless(vi->pan == -128, "set pan left");

	xmp_smix_channel_pan(opaque, 0, 0xff);
	xmp_play_frame(opaque);
	fail_unless(vi->pan == 127, "set pan right");

	xmp_end_player(opaque);
	xmp_end_smix(opaque);

	xmp_start_player(opaque, 44100, 0);

	/* Try to set pan after deinit */
	xmp_smix_channel_pan(opaque, 0, 0x78);
	fail_unless(ret == -XMP_ERROR_INVALID, "set pan after deinit");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
