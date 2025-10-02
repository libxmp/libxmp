#include "test.h"

/* Ice! magic with 8-bit stream, no filter.
 * Unambiguous stream format--aspects of the initial bit read
 * immediately clarify that this is an 8-bit stream. */

TEST(test_depack_ice_22)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");

	ret = xmp_load_module(c, "data/ice22.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "55904b7ab3406535800db05ac7bfe4b3");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
