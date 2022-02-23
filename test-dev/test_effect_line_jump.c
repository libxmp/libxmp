#include "test.h"
#include "../src/effects.h"


TEST(test_effect_line_jump)
{
	struct pos_row {
		int pos;
		int row;
	};
	static const struct pos_row vals[] = {
		{ 0,  0 },	/* jump to 16 (fwd) */
		{ 0, 16 },	/* jump to 12 (back) */
		{ 0, 12 },
		{ 0, 13 },
		{ 0, 14 },
		{ 0, 15 },	/* break then line jump: break with line jump target */
		{ 1,  0 },	/* jump to 63 (last valid) */
		{ 1, 63 },
		{ 2,  0 },	/* pattern jump then line jump: pattern jump with line jump target */
		{ 3, 16 },	/* line jump then break: line jump to the break target */
		{ 3, 63 },
		{ 4,  0 },	/* line jump then pattern jump: pattern jump to line 0 */
		{ 5,  0 },
		{ 6,  0 },	/* infinite loop -- libxmp should exit eventually. */
		{ 6,  0 },
		{ 6,  0 },
		{ 6,  0 },
	};

	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	int i, ret;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	create_simple_module(ctx, 1, 7);

	new_event(ctx, 0, 0, 0, 0, 0, 0, FX_LINE_JUMP, 0x10, FX_SPEED, 1);
	new_event(ctx, 0, 15, 0, 0, 0, 0, FX_IT_BREAK, 0x20, 0, 0);
	new_event(ctx, 0, 15, 1, 0, 0, 0, FX_LINE_JUMP, 0x00, 0, 0);
	new_event(ctx, 0, 16, 0, 0, 0, 0, FX_LINE_JUMP, 0x0c, 0, 0);
	new_event(ctx, 1, 0, 0, 0, 0, 0, FX_LINE_JUMP, 0x3f, 0, 0);
	new_event(ctx, 2, 0, 0, 0, 0, 0, FX_JUMP, 0x03, 0, 0);
	new_event(ctx, 2, 0, 1, 0, 0, 0, FX_LINE_JUMP, 0x10, 0, 0);
	new_event(ctx, 3, 16, 0, 0, 0, 0, FX_LINE_JUMP, 0x20, 0, 0);
	new_event(ctx, 3, 16, 1, 0, 0, 0, FX_IT_BREAK, 0x3f, 0, 0);
	new_event(ctx, 4, 0, 0, 0, 0, 0, FX_LINE_JUMP, 0x3f, 0, 0);
	new_event(ctx, 4, 0, 1, 0, 0, 0, FX_JUMP, 0x05, 0, 0);
	new_event(ctx, 5, 0, 0, 0, 0, 0, FX_IT_BREAK, 0x00, 0, 0);
	new_event(ctx, 6, 0, 0, 0, 0, 0, FX_LINE_JUMP, 0x00, 0, 0);

	libxmp_free_scan(ctx);
	for (i = 0; i < 7; i++) {
		set_order(ctx, i, i);
	}
	libxmp_prepare_scan(ctx);
	ret = libxmp_scan_sequences(ctx);
	fail_unless(ret == 0, "scan error");

	xmp_start_player(opaque, 8000, 0);

	for (i = 0; i < ARRAY_SIZE(vals); i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		fail_unless(info.pos == vals[i].pos, "line jump error");
		fail_unless(info.row == vals[i].row, "line jump error");
	}

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
