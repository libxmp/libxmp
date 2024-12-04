#include "test.h"


TEST(test_fuzzer_dt_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused hangs and massive RAM consumption in the Digital Tracker
	 * loader due to a missing channel count bounds check.
	 */
	ret = xmp_load_module(opaque, "data/f/load_dt_invalid_channel_count.dtm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (channel count)");

	/* This input caused leaks in the Digital Tracker loader due to attempting
	 * to load an invalid number of instruments.
	 */
	ret = xmp_load_module(opaque, "data/f/load_dt_invalid_instrument_count.dtm.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (instrument count)");

	/* This input caused signed integer overflows in the DTM loader due
	 * to poor bounding of sample lengths and invalid sample loops.
	 */
	ret = xmp_load_module(opaque, "data/f/load_dt_invalid_sample_loop.dtm");
	fail_unless(ret == 0, "module load (sample loop)");

	/* This input caused signed integer overflows in the DTM test function
	 * due to badly handling "negative" chunk lengths.
	 */
	ret = xmp_load_module(opaque, "data/f/load_dt_invalid_header_size.dtm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (header size)");

	xmp_free_context(opaque);
}
END_TEST
