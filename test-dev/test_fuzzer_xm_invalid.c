#include "test.h"


TEST(test_fuzzer_xm_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input contains some unusual/invalid patterns that originally
	 * caused undefined behavior in the OXM test function. It now covers
	 * related pattern checks in the XM loader. */
	ret = xmp_load_module(opaque, "data/f/load_xm_invalid_pattern_length.xm");
	fail_unless(ret == -XMP_ERROR_LOAD, "depacking (invalid_pattern_length)");

	/* This input caused undefined behavior in the XM loader due to
	 * having a broken instrument header size bounds check. */
	ret = xmp_load_module(opaque, "data/f/load_xm_invalid_instsize.xm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_instsize)");

	xmp_free_context(opaque);
}
END_TEST
