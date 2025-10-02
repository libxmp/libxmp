#include "test.h"

/* ICE! magic with 8-bit stream, variable length bitplane filter enabled.
 * Pack-Ice 2.4 has broken filter verification, so these need to be
 * packed and unpacked with Pack-Ice 2.34. */

TEST(test_depack_ice_234_filter)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");
	ret = xmp_load_module(c, "data/ice234_filter.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "f71ca4bdee5b71b844c2e9c1228d0e53");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
