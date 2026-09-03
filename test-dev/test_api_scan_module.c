#include "test.h"
#include "../src/effects.h"

TEST(test_api_scan_module)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_module_info minfo;
	struct xmp_frame_info info;
	int ret;
	int i;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	/* try to scan before loading */
	xmp_scan_module(opaque);

 	create_simple_module(ctx, 2, 2);

	new_event(ctx, 0, 0, 0, 0, 0, 0, FX_SPEED, 0x03, 0, 0);
	new_event(ctx, 0, 1, 0, 0, 0, 0, FX_SPEED, 0x1f, 0, 0);
	new_event(ctx, 0, 2, 0, 0, 0, 0, FX_SPEED, 0x02, 0, 0);
	new_event(ctx, 0, 3, 0, 0, 0, 0, FX_SPEED, 0x20, 0, 0);
	new_event(ctx, 0, 4, 0, 0, 0, 0, FX_SPEED, 0x80, 0, 0);

	xmp_scan_module(opaque);

	xmp_start_player(opaque, 44100, 0);

	for (i = 0; i < (3 + 0x1f + 3 * 2); i++) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		fail_unless(info.total_time == 5720, "total time error");
	}

	xmp_release_module(opaque);

	/* Load something with an absurd number of sequences. */
	ret = xmp_load_module(opaque, "data/scan_240_seq.it");
	fail_unless(ret == 0, "load module");

	xmp_scan_module(opaque);

	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.num_sequences == 240, "should have 240 sequences");

	for (i = 0; i < minfo.num_sequences; i++) {
		fail_unless(minfo.seq_data[i].entry_point == i, "entry point");
	}
	xmp_release_module(opaque);

	/* Invalid patterns followed by valid pattern -> 1 sequence */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0x63);
	set_order(ctx, 1, XMP_MARK_SKIP);
	set_order(ctx, 2, 0);
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_release_module(opaque);

	/* Invalid patterns with no valid pattern -> 1 sequence */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0x12);
	set_order(ctx, 1, 0x34);
	set_order(ctx, 2, 0xde);
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	/* TODO: libxmp_prepare_scan is still clobbering length in this case. */
	/*fail_unless(minfo.mod->len == 3, "should have 3 positions");*/
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	/*fail_unless(minfo.mod->len == 3, "should have 3 positions");*/
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	/*fail_unless(minfo.mod->len == 3, "should have 3 positions");*/
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");

	xmp_release_module(opaque);

	/* Valid, end marker, valid -> 1 sequence MOD, 2 sequences S3M */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0);
	set_order(ctx, 1, XMP_MARK_END);
	set_order(ctx, 2, 1);
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 2, "should have 2 sequences");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_release_module(opaque);

	/* End marker, valid -> 1 sequence MOD, 2 sequences S3M */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, XMP_MARK_END);
	set_order(ctx, 1, XMP_MARK_END);
	set_order(ctx, 2, 0);
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 2, "should have 2 sequences");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "gets added to sequence 1 apparently...");
	xmp_set_position(opaque, 2);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_release_module(opaque);

	/* Invalid, end marker, valid -> 1 sequence MOD, 2 sequences S3M */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, 0x63);
	set_order(ctx, 1, XMP_MARK_END);
	set_order(ctx, 2, 0);
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 3, "should have 3 positions");
	fail_unless(minfo.num_sequences == 2, "should have 2 sequences");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");
	xmp_set_position(opaque, 2);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == 0, "should play");

	xmp_release_module(opaque);

	/* Length 0 -> 1 sequence, OK, -XMP_END on xmp_play_frame */
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	minfo.mod->len = 0;
	libxmp_prepare_scan(ctx);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 0, "should have 0 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");

	xmp_scan_module(opaque);
	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 0, "should have 0 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");

	xmp_restart_module(opaque);
	xmp_set_player(opaque, XMP_PLAYER_MODE, XMP_MODE_S3M); /* rescan */
	xmp_get_module_info(opaque, &minfo);
	fail_unless(minfo.mod->len == 0, "should have 0 positions");
	fail_unless(minfo.num_sequences == 1, "should have 1 sequence");
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");
	xmp_set_position(opaque, 1);
	ret = xmp_play_frame(opaque);
	fail_unless(ret == -XMP_END, "nothing to play");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
