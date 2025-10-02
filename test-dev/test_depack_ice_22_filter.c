#include "test.h"

/* Ice! magic with 8-bit stream, bitplane filter enabled. */

TEST(test_depack_ice_22_filter)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");
	ret = xmp_load_module(c, "data/ice22_filter.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "0dfff3d3dd2f9905d4d7e541a9c8e402");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
