#include "test.h"

/* This input OXM caused NULL dereferences in stb-vorbis.
 */

TEST(test_fuzzer_xm_vorbis_crash)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_xm_vorbis_crash.oxm");
	fail_unless(ret == -XMP_ERROR_DEPACK, "module load");

	xmp_free_context(opaque);
}
END_TEST
