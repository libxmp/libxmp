#include "test.h"

/* Inputs that may cause issues in the sample scanning loop in the MED4 loader.
 */

TEST(test_fuzzer_med4_invalid_sample)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* Sample of length INT_MAX */
	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (1)");

	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample2.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (2)");

	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample3.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (3)");

	/* Synth of length INT_MAX but with otherwise valid data */
	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample4.med");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (4)");

	/* When interpreted normally, this MED should load successfully with
	 * one hybrid instrument and one skipped unknown type. A bug caused
	 * libxmp to interpret it incorrectly after counting samples (treating
	 * extra data in the first hybrid as a second hybrid), which caused
	 * out-of-bounds heap writes loading the second sample.
	 */
	ret = xmp_load_module(opaque, "data/f/load_med4_invalid_sample5.med");
	fail_unless(ret == 0, "module load (5)");

	xmp_free_context(opaque);
}
END_TEST
