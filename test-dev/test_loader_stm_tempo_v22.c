#include "test.h"

/* NOTE: ST2 has very bizarre tempo handling. libxmp currently plays this
 * module incorrectly due to various limitations, hence this is a loader
 * test to verify approximate behavior instead of a player test.
 */

TEST(test_loader_stm_tempo_v22)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_stm_tempo_v22.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/st22_tempo.stm");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
