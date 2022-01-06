#include "test.h"

/* Not a fuzzer file, but make sure libxmp rejects PAK compression.
 */

TEST(test_fuzzer_depack_arc_pak10)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/depack_arc_pak10");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking");

	xmp_free_context(opaque);
}
END_TEST
