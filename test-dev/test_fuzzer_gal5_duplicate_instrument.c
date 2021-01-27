#include "test.h"

/* This input caused memory leaks in the Galaxy 5.0 loader due to
 * containing duplicate instruments.
 */

TEST(test_fuzzer_gal5_duplicate_instrument)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_gal5_duplicate_instrument.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
