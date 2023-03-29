#include "test.h"

/* MPT 1.16 pre-amp scaling logarithmically reduces the effective
 * mix volume of modules based on the number of channels they contain,
 * from 100% (<=5 channels) to 50% (30+). The input module has 100 mix
 * volume and 32 channels, so the effective mix volume should be 50.
 *
 * (Note: this isn't quite true for pre-amp levels higher than 128;
 * libxmp currently only supports a fixed value of 128, and does
 * not attempt to emulate other aspects of MPT's pre-amp routine.
 * This is currently applied in the loader instead of the mixer.)
 */

TEST(test_mixer_mpt116_preamp)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct module_data *m;

	opaque = xmp_create_context();
	xmp_load_module(opaque, "data/mpt116_32chn.it");

	ctx = (struct context_data *)opaque;
	m = &ctx->m;

	fail_unless(m->mvol == 50, "mix volume not adjusted correctly");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
