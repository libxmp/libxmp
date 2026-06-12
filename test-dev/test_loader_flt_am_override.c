#include "test.h"

/* Also see test_player_flt_am_override for the player side of this test.
 *
 * 1) When an instrument has a sample and an AM instrument both specified,
 *    the AM instrument takes precedence (instrument 2; 00-05, left).
 * 2) When an AM instrument is truncated in the .NT file, assume 0 for the
 *    truncated fields P.FALL, V.AMP, V.SPD, FQ (instrument 4; 00-05, right).
 * 3) AM instruments overriding samples should not affect how the sample
 *    data is loaded from the module file (instrument 3; 06-07, right).
 *    In other words, when instrument 2 is loaded, its sample should either
 *    be fully loaded or fully skipped, and instrument 3 should sound
 *    identical to instrument 1.
 */

TEST(test_loader_flt_am_override)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_flt_am_override.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/flt_am_override.mod");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
