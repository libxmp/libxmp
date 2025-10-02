#include "test.h"

/* ICE! magic with 8-bit stream, bitplane filter enabled.
 * The input file was 32259 bytes, the maximum allowed for
 * the bitplane filter, but Pack-Ice doesn't filter the whole
 * file--it leaves the first (N-32000) bytes unfiltered and
 * only applies the default filter to the end. */

TEST(test_depack_ice_231_filterext)
{
	xmp_context c;
	struct xmp_module_info info;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");
	ret = xmp_load_module(c, "data/ice231_filterext.mod");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	ret = compare_md5(info.md5, "d90a11532023d39f9806b994fc77109f");
	fail_unless(ret == 0, "MD5 error");

	xmp_release_module(c);
	xmp_free_context(c);
}
END_TEST
