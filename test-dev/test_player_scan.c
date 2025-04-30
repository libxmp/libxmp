#include "test.h"

static void check_sequences(xmp_context opaque, const char *test)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	struct player_data *p = &ctx->p;
	char buf[128];
	int i;

	/* There should be no junk sequences after the valid ones. */
	for (i = m->num_sequences; i < MAX_SEQUENCES; i++) {
		snprintf(buf, sizeof(buf), "junk entry point in '%s' @ %d", test, i);
		fail_unless(m->seq_data[i].entry_point == 0, buf);
		snprintf(buf, sizeof(buf), "junk duration in '%s' @ %d", test, i);
		fail_unless(m->seq_data[i].duration == 0, buf);
	}
	/* There should be no sequences past the last sequence. */
	for (i = 0; i < m->mod.len; i++) {
		snprintf(buf, sizeof(buf), "bad sequence '%02x' in '%s' @ %d",
			 p->sequence_control[i], test, i);
		fail_unless(p->sequence_control[i] < m->num_sequences ||
			p->sequence_control[i] == NO_SEQUENCE, buf);
	}
}

TEST(test_player_scan)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	int ret;

	opaque = xmp_create_context();
	fail_unless(opaque != NULL, "can't create context");
	ctx = (struct context_data *)opaque;

	ret = xmp_load_module(opaque, "data/ode2ptk.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_start_player(opaque, 44100, 0);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.total_time == 85472, "incorrect total time");
	check_sequences(opaque, "ode2ptk");

	/* This is a very long OctaMED module with 256 orders of 3200 rows,
	 * at BPM 28/LPB 1, speed 32. Currently the scan can't report the
	 * length accurately, but it should at least not reject this module
	 * and try to report a usable value. */
	ret = xmp_load_module(opaque, "data/longest.med");
	fail_unless(ret == 0, "can't load module (2)");

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	xmp_get_frame_info(opaque, &info);
	fail_unless(info.total_time == INT_MAX, "incorrect total time");
	check_sequences(opaque, "longest.med");

	/* Make sure S3M/IT end markers aren't assigned invalid sequences. */
	xmp_release_module(opaque);
	create_simple_module(ctx, 2, 2);
	libxmp_free_scan(ctx);
	set_order(ctx, 0, XMP_MARK_SKIP);
	set_order(ctx, 1, 0);
	set_order(ctx, 2, XMP_MARK_END);
	set_order(ctx, 3, XMP_MARK_END); /* May not be given a real sequence */
	set_quirk(ctx, QUIRK_MARKER, READ_EVENT_IT);
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);
	check_sequences(opaque, "s3m/it markers");

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
