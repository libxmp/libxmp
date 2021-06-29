#include "test.h"

/* This input caused uninitialized reads in the RTM loader due to a
 * missing EOF check after reading the panning table.
 */

TEST(test_fuzzer_rtm_truncated)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_rtm_truncated.rtm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
