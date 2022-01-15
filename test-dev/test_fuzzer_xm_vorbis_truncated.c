#include "test.h"


TEST(test_fuzzer_xm_vorbis_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* These inputs caused hangs, leaks, or crashes in stb_vorbis due
	 * to missing or broken EOF checks in start_decoder. */
	ret = xmp_load_module(opaque, "data/f/load_xm_vorbis_truncated.oxm");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated)");

	ret = xmp_load_module(opaque, "data/f/load_xm_vorbis_truncated2.oxm");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated2)");

	ret = xmp_load_module(opaque, "data/f/load_xm_vorbis_truncated3.oxm");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated3)");

	ret = xmp_load_module(opaque, "data/f/load_xm_vorbis_truncated4.oxm");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (truncated4)");

	xmp_free_context(opaque);
}
END_TEST
