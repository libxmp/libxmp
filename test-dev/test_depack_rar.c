#include "test.h"


TEST(test_depack_rar)
{
#ifdef HAVE_LIBUNARR
	xmp_context c;
	struct xmp_module_info info;
	FILE *f;
	int ret;

	c = xmp_create_context();
	fail_unless(c != NULL, "can't create context");
	ret = xmp_load_module(c, "data/ponylips.rar");
	fail_unless(ret == 0, "can't load module");

	xmp_get_module_info(c, &info);

	f = fopen("data/format_mod_notawow.data", "r");

	ret = compare_module(info.mod, f);
	fail_unless(ret == 0, "RARed module not correctly loaded");

	xmp_release_module(c);
	xmp_free_context(c);
#endif
}
END_TEST
