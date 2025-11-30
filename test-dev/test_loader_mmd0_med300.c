#include "test.h"

/* MED 3.00 / OctaMED 1.00 saves modules with s_ext_entrsz 2.
 * This is in spite of the documentation from later versions
 * implying that the smallest s_ext_entrsz is 4. Do not load
 * later instruments' hold/decay values as finetune.
 */

TEST(test_loader_mmd0_med300)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_mmd0_med300.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/med_s_ext_entrsz_2.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
