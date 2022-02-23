#include "test.h"

/* Digital Symphony groups patterns into chunks of 2000.
 */

TEST(test_loader_sym_4096pat)
{
	xmp_context opaque;
	struct xmp_module_info info;
	static const int idx[8] = {
		0, 0xff9, 0xffa, 0xffb, 0xffc, 0xffd, 0xffe, 0xfff };
	int ret;
	int i;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/4096_patterns.dsym");
	fail_unless(ret == 0, "module load");

	xmp_get_module_info(opaque, &info);

	/* Data file is too big so just test the relevant portions. */
	fail_unless(info.mod->pat == 1, "patterns mismatch");
	fail_unless(info.mod->chn == 8, "channels mismatch");
	for (i = 0; i < 8; i++)
		fail_unless(info.mod->xxp[0]->index[i] == idx[i], "tracks mismatch");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
