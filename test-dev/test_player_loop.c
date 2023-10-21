#include "test.h"
#include "../src/effects.h"

#define IT_SKIP 0xfe
#define IT_END 0xff

struct test_seq
{
	int entry;
	int ticks;
};

TEST(test_player_loop)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	int ret, i, j, pat, pos, seq;
	int remote_end, jump_skip_end;
	struct test_seq test_seq[16];

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	create_simple_module(ctx, 1, 16);
	libxmp_free_scan(ctx);
	set_quirk(ctx, QUIRK_MARKER, READ_EVENT_IT);
	pat = 0;
	pos = 0;
	seq = 0;

	/* Main sequence */
	new_event(ctx, pat, 0, 0, 0, 0, 0, FX_BREAK, 0, 0, 0);
	test_seq[seq].entry = pos;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), (pat++));
	remote_end = pos; /* for use by a later sequence */
	set_order(ctx, (pos++), IT_END);
	seq++;

	/* Sequence: pattern, end */
	new_event(ctx, pat, 0, 0, 0, 0, 0, FX_BREAK, 0, 0, 0);
	test_seq[seq].entry = pos;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), (pat++));
	set_order(ctx, (pos++), IT_END);
	seq++;

	/* Sequence: skip, pattern, end */
	new_event(ctx, pat, 0, 0, 0, 0, 0, FX_BREAK, 0, 0, 0);
	test_seq[seq].entry = pos + 1;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), IT_SKIP);
	set_order(ctx, (pos++), (pat++));
	set_order(ctx, (pos++), IT_END);
	seq++;

	/* Sequence: pattern jump into end marker */
	new_event(ctx, pat, 0, 0, 0, 0, 0, FX_JUMP, remote_end, 0, 0);
	test_seq[seq].entry = pos;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), (pat++));
	set_order(ctx, (pos++), IT_END);
	seq++;

	/* Sequence: jump into skip into end. This should be last. */
	jump_skip_end = pat;
	test_seq[seq].entry = pos;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), (pat++));
	seq++;
	/* Sequence: do it again, because the reuse is what caused a bug. */
	test_seq[seq].entry = pos;
	test_seq[seq].ticks = 6;
	set_order(ctx, (pos++), jump_skip_end);
	seq++;
	/* Target */
	new_event(ctx, jump_skip_end, 0, 0, 0, 0, 0, FX_JUMP, pos, 0, 0);
	set_order(ctx, (pos++), IT_SKIP);
	/* intentionally no terminating IT_END. */

	ctx->m.mod.len = pos;
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	ret = xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	fail_unless(ret == 0, "failed to start player");

	for (i = 0; i < seq; i++) {
		xmp_restart_module(opaque);

		ret = xmp_set_position(opaque, test_seq[i].entry);
		if (i > 0) {
			fail_unless(ret == test_seq[i].entry, "failed to set position");
		} else {
			fail_unless(ret == -1, "failed to set position");
		}

		xmp_get_frame_info(opaque, &info);
		fail_unless(info.sequence == i, "entered wrong sequence");

		ctx->p.loop_count = 0;

		for (j = 0; j <= test_seq[i].ticks; j++) {
			fail_unless(info.loop_count == 0, "loop occurred too early");

			xmp_play_frame(opaque);

			xmp_get_frame_info(opaque, &info);
			fail_unless(info.sequence == i, "wrong sequence");
		}
		fail_unless(info.loop_count == 1, "failed to detect loop");
	}
	xmp_free_context(opaque);
}
END_TEST
