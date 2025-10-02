#include "test.h"

/* Ice! magic with 8-bit stream, no filter.
 * Ambiguous stream format--may attempt depacking as either. */

TEST(test_depack_ice_22_ambiguous)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");

	ret = xmp_load_module(c, "data/ice22_ambiguous.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "84b944cce487836dd9c2a0e59e23516c");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
