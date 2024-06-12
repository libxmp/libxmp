#include "test.h"
#include "../src/loaders/loader.h"

TEST(test_effect_ef_invert_loop)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct module_data *m;
	HIO_HANDLE *h;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	m = &ctx->m;

	create_simple_module(ctx, 2, 2);
	set_quirk(ctx, QUIRK_PROTRACK, READ_EVENT_MOD);
	h = hio_open("data/sample-square-8bit.raw", "rb");
	fail_unless(h != NULL, "can't open sample file");

	libxmp_free_sample(&m->mod.xxs[0]);
	m->mod.xxs[0].len = 40;
	m->mod.xxs[0].lps = 0;
	m->mod.xxs[0].lpe = 40;
	libxmp_load_sample(m, h, 0, &m->mod.xxs[0], NULL);
	hio_close(h);

	new_event(ctx, 0, 0, 0, 49, 1, 0, 0x0e, 0xfe, 0x0f, 1);

	xmp_start_player(opaque, 16000, XMP_FORMAT_MONO);
	xmp_set_player(opaque, XMP_PLAYER_INTERP, XMP_INTERP_NEAREST);

	compare_mixer_samples_ext(ctx, "data/invloop.data", 0, 6);

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
