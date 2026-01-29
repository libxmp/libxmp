#include "test.h"
#include "../src/effects.h"

/* #define GENERATE */

TEST(test_mixer_downmix_32bit)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct xmp_frame_info info;
	FILE *f;
	int i, j;
	long val;

#ifndef GENERATE
	f = fopen("data/downmix.data", "r");
#else
	f = fopen("data/downmix.data", "w");
#endif

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;

	xmp_load_module(opaque, "data/test.xm");

	new_event(ctx, 0, 0, 0, 48, 1, 0, 0x0f, 2, 0, 0);

	xmp_start_player(opaque, 22050, XMP_FORMAT_MONO | XMP_FORMAT_32BIT);

	for (i = 0; i < 2; i++) {
		int32 *b;
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		b = (int32 *)info.buffer;
		for (j = 0; j < info.buffer_size / 4; j++) {
#ifndef GENERATE
			int ret = fscanf(f, "%ld", &val);
			fail_unless(ret == 1, "read error");
			fail_unless(b[j] == val, "downmix error");
#else
			fprintf(f, "%ld\n", (long)b[j]);
#endif
		}
	}

	fclose(f);
#ifdef GENERATE
	fail_unless(0, "generate enabled!");
#endif

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
