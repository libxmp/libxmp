#include "test.h"

TEST(test_api_add_channels)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	int state, ret, i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	state = xmp_get_player(opaque, XMP_PLAYER_STATE);
	fail_unless(state == XMP_STATE_UNLOADED, "state error");

	create_simple_module(ctx, 1, 1);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0);

	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	ret = xmp_add_channels(opaque, -1, 2);
	fail_unless(ret == 0, "unable to append two channels");

	/*
	ret = xmp_add_channels(opaque, 0, -1);
	fail_unless(ret == 0, "unable to remove a channel");
	*/

	/*
	ret = xmp_add_channels(opaque, 0, XMP_MAX_CHANNELS + 1);
	fail_unless(ret == -XMP_ERROR_INVALID, "erroneously succeeded for invalid number of channels");
	*/

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
