#include "test.h"

/* Ice! magic with 32-bit stream, no filter.
 * This file has an unambiguous stream format--aspects of the initial
 * bit read immediately clarify that this is an 32-bit stream. */

TEST(test_depack_ice_21)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");

	ret = xmp_load_module(c, "data/ice21.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "84b944cce487836dd9c2a0e59e23516c");
	fail_unless(ret == 0, "MD5 error ");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
