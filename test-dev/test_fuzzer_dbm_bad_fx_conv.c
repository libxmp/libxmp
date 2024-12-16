#include "test.h"

TEST(test_fuzzer_dbm_bad_fx_conv)
{
	/* This DBM has a truncated but valid(?) pattern that caused the
	 * bad bounding in the DBM loader to terminate before converting
	 * effects. If effects conversion doesn't occur, the effect loaded
	 * at the time of making this test is A2h (Ice Speed) and it causes
	 * division-by-zero errors during playback. Just make sure this
	 * effect is filtered from the loaded pattern data.
	 */
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/f/load_dbm_bad_fx_conv.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_dbm_bad_fx_conv.dbm");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
