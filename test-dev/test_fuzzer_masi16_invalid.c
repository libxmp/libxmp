#include "test.h"


TEST(test_fuzzer_masi16_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input crashed the PSM loader due to containing a samples
	 * count over 64, which overflowed the sample offsets array.
	 */
	ret = xmp_load_module(opaque, "data/f/load_masi16_invalid.psm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (1)");

	/* The maximum sample length supported in PS16 PSMs is 64k.
	 */
	ret = xmp_load_module(opaque, "data/f/load_masi16_invalid2.psm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (2)");

	/* Contains a duplicate sample that causes an uninitialized read
	 * of the sample offsets array.
	 */
	ret = xmp_load_module(opaque, "data/f/load_masi16_invalid3.psm");
	fail_unless(ret == 0, "module load (3)");

	xmp_free_context(opaque);
}
END_TEST
