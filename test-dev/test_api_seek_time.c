#include "test.h"

int pos_ode2ptk[] = {
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 
	2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 
	5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 
	10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 
	13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 
	17, 17, 17, 17, 17, 17, 17, 17, 17, 17
};

int pos_dlr[] = {
	0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9,
	9, 9, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13,
	13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16,
	17, 17, 18, 18, 18, 18, 18, 18, 19, 19, 19, 20, 20,
	20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23,
	24, 24, 24, 24, 25, 25, 25, 25, 26, 26, 26, 27, 27
};

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
	int ret;
	int i;

	/* Seek ode2ptk */

	ctx = xmp_create_context();
	xmp_load_module(ctx, "data/ode2ptk.mod");
	xmp_start_player(ctx, 8000, 0);

	for (i = 0; i < 100; i++) {
		ret = xmp_seek_time(ctx, i * 1000);
		fail_unless(ret == pos_ode2ptk[i], "seek error");
	}
	test_order_times(ctx, "ode2ptk");

	xmp_release_module(ctx);
	xmp_free_context(ctx);

	/* Seek dans le rue */

	ctx = xmp_create_context();
	xmp_load_module(ctx, "data/m/xyce-dans_la_rue.xm");
	xmp_start_player(ctx, 8000, 0);

	for (i = 0; i < 100; i++) {
		ret = xmp_seek_time(ctx, i * 1000);
		fail_unless(ret == pos_dlr[i], "seek error");
	}
	test_order_times(ctx, "dans_la_rue");

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
