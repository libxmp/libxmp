#include "test.h"

/* 1) Finetune should be applied the same to all synth/hybrid
 *    subinstruments.
 * 2) No -fsanitize=shift-base warnings/errors should occur.
 */

TEST(test_loader_mmd0_finetune)
{
	xmp_context opaque;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	f = fopen("data/format_mmd0_finetune.data", "r");

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/finetune.med");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "format not correctly loaded");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
