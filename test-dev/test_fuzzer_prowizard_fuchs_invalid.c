#include "test.h"


TEST(test_fuzzer_prowizard_fuchs_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Fuchs Tracker
	 * loader due to a faulty bounds check on the pattern length. */
	ret = xmp_load_module(opaque, "data/f/prowizard_fuchs_invalid_pattern_length.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_length)");

	xmp_free_context(opaque);
}
END_TEST
