#include "test.h"

/* Ice! magic with 32-bit stream, no filter.
 * Ambiguous stream format--may attempt depacking as either. */

TEST(test_depack_ice_21_ambiguous)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");

	ret = xmp_load_module(c, "data/ice21_ambiguous.xm");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "656af451d82f9c2ccc89c5c8ced8d848");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
