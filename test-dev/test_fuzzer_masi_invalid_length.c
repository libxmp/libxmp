#include "test.h"

/* This input caused heap corruption in the MASI loader due to
 * a missing sanity check on module length.
 */

TEST(test_fuzzer_masi_invalid_length)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_masi_invalid_length.psm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
