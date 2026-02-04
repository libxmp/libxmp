#include "test.h"

/* Test xmp_seek_time and xmp_seek_time_frame. */

struct pos_row_frame {
	int pos;
	int row;
	int frame;
};

static const int pos_ode2ptk[] = {
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
	2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5,
	5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9,
	10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13,
	13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};

static const int pos_dlr[] = {
	0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9,
	9, 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13,
	13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16,
	17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 20, 20,
	20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23,
	24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27
};


static const int seek_frame_times[] = {
	0, -10, -10000, INT_MIN, 10000, 32767, INT_MAX, 12345
};

static const struct pos_row_frame frame_ode2ptk[] = {
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  2,  1, 2 },
	{  5, 63, 1 },
	{ 17, 39, 7 },
	{  2, 20, 5 },
};

static const struct pos_row_frame frame_dlr[] = {
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  0,  0, 0 },
	{  3, 13, 0 },
	{  9, 23, 2 },
	{ 44, 62, 2 },
	{  3, 53, 2 },
};

static void test_seek_orders(xmp_context opaque, const int *table,
	const char *test)
{
	char buf[128];
	int ret, i;

	for (i = 0; i < 100; i++) {
		ret = xmp_seek_time(opaque, i * 1000);
		snprintf(buf, sizeof(buf),
			"xmp_seek_time to %ds failed in '%s': expected ord %d, got %d",
			i, test, table[i], ret);
		fail_unless(ret == table[i], buf);

		ret = xmp_seek_time_frame(opaque, i * 1000);
		snprintf(buf, sizeof(buf),
			"xmp_seek_time_frame to %ds failed in '%s': expected ord %d, got %d",
			i, test, table[i], ret);
		fail_unless(ret == table[i], buf);
	}
}

/* Because all API seeks leave stale data in the row/frame vars,
 * one frame needs to be played after every seek to get accurate info. */
static void test_seek_frames(xmp_context opaque,
	const struct pos_row_frame *table, const char *test)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	struct player_data *p = &ctx->p;
	char buf[128];
	size_t i;
	int ret;

	for (i = 0; i < ARRAY_SIZE(seek_frame_times); i++) {
		ret = xmp_seek_time(opaque, seek_frame_times[i]);
		xmp_play_frame(opaque);
		snprintf(buf, sizeof(buf),
			"xmp_seek_time to %ds failed in '%s': expected %d %d 0, got %d %d %d",
			seek_frame_times[i], test,
			table[i].pos, m->xxo_info[ret].start_row,
			ret, p->row, p->frame);
		fail_unless(ret == table[i].pos, buf);
		fail_unless(p->row == m->xxo_info[ret].start_row, buf);
		fail_unless(p->frame == 0, buf);

		ret = xmp_seek_time_frame(opaque, seek_frame_times[i]);
		xmp_play_frame(opaque);
		snprintf(buf, sizeof(buf),
			"xmp_seek_time_frame to %ds failed in '%s': expected %d %d %d, got %d %d %d",
			seek_frame_times[i], test,
			table[i].pos, table[i].row, table[i].frame,
			ret, p->row, p->frame);
		fail_unless(ret == table[i].pos, buf);
		fail_unless(p->row == table[i].row, buf);
		fail_unless(p->frame == table[i].frame, buf);
	}
}

static void test_order_times(xmp_context opaque, const char *test)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct module_data *m = &ctx->m;
	char buf[128];
	double time;
	int ret, i;

	/* Should be able to seek to the exact order time without issue.
	 * For extremely long modules (longest.med), this won't seek to
	 * the correct spot currently due to API limitations. */
	for (i = 0; i < m->mod.len; i++) {
		/* API expects rounded values due to having been
		 * designed to handle int (rather than double). */
		time = m->xxo_info[i].time;
		CLAMP(time, 0.0, (double)INT_MAX);
		ret = xmp_seek_time(opaque, (int)time);

		/* Seek (but don't check) overflowed times and markers
		 * so they get sanitized. */
		if (time >= (double)INT_MAX ||
		    m->mod.xxo[i] == XMP_MARK_SKIP ||
		    m->mod.xxo[i] == XMP_MARK_END) {
			continue;
		}
		snprintf(buf, sizeof(buf),
			"time seek to %.6f failed in '%s' @ %d (got %d)",
			time, test, i, ret);
		fail_unless(ret == i, buf);
	}
}

TEST(test_api_seek_time)
{
	xmp_context ctx;

	/* Seek ode2ptk */

	ctx = xmp_create_context();
	xmp_load_module(ctx, "data/ode2ptk.mod");
	xmp_start_player(ctx, 8000, 0);

	test_seek_orders(ctx, pos_ode2ptk, "ode2ptk");
	test_order_times(ctx, "ode2ptk");
	test_seek_frames(ctx, frame_ode2ptk, "ode2ptk");

	xmp_release_module(ctx);
	xmp_free_context(ctx);

	/* Seek dans le rue */

	ctx = xmp_create_context();
	xmp_load_module(ctx, "data/m/xyce-dans_la_rue.xm");
	xmp_start_player(ctx, 8000, 0);

	test_seek_orders(ctx, pos_dlr, "dans_la_rue");
	test_order_times(ctx, "dans_la_rue");
	test_seek_frames(ctx, frame_dlr, "dans_la_rue");

	xmp_release_module(ctx);
	xmp_free_context(ctx);

	/* Seek longest.med */

	ctx = xmp_create_context();
	xmp_load_module(ctx, "data/longest.med");
	xmp_start_player(ctx, 8000, 0);

	test_order_times(ctx, "longest.med");

	xmp_release_module(ctx);
	xmp_free_context(ctx);
}
END_TEST
