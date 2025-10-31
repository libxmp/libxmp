#include "test.h"

/* -CJ- hacked magic with 32-bit stream, no filter.
 * (Apparently) these hacked magics are always 32-bit streams. */

TEST(test_depack_ice_21_cj)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");

	ret = xmp_load_module(c, "data/ice21_cj.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "55904b7ab3406535800db05ac7bfe4b3");
	fail_unless(ret == 0, "MD5 error ");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
