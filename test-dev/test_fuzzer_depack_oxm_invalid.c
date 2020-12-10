#include "test.h"

/* This input caused an out-of-bounds write in stb_vorbis due to
 * a missing negative return value check.
 */

TEST(test_fuzzer_depack_oxm_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/depack_oxm_invalid.oxm");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
