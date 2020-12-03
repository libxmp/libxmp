#include "test.h"

/* This input crashed the ULT loader due to invalid track
 * data packing.
 */

TEST(test_fuzzer_ult_invalid_tracks)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_ult_invalid_tracks.ult");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
