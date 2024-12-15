#include "test.h"

TEST(test_fuzzer_mgt_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused signed overflows in the Megatracker loader
	 * when calculating track pointer offsets due to a track pointer
	 * table offset near INT_MAX.
	 */
	ret = xmp_load_module(opaque, "data/f/load_mgt_invalid_track_offset.mgt");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
