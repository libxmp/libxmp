#include "test.h"

/* This input crashed the FNK loader due to containing a channel
 * count of 0.
 */

TEST(test_fuzzer_fnk_channels_bound)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_fnk_channels_bound.fnk");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
