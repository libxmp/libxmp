#include "test.h"

TEST(test_api_set_row)
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

	ret = xmp_set_row(opaque, 0);
	fail_unless(ret == -XMP_ERROR_STATE, "state check error");

	create_simple_module(ctx, 1, 1);
	set_order(ctx, 0, 0);

	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	xmp_start_player(opaque, 44100, 0);
	fail_unless(p->row == 0, "didn't start at row 0");

	ret = xmp_set_row(opaque, 1);
	fail_unless(ret == 1, "return value error");
	xmp_play_frame(opaque);
	fail_unless(p->row == 1, "didn't set row 1");

	ret = xmp_set_row(opaque, 64);
	fail_unless(ret == -XMP_ERROR_INVALID, "return value error");
}
END_TEST
