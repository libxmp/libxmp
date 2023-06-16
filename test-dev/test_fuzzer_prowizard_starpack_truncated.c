#include "test.h"


TEST(test_fuzzer_prowizard_starpack_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Startrekker Packer
	 * test function due to inadequate PW_REQUEST_DATA for the pattern check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_starpack_truncated");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");

	xmp_free_context(opaque);
}
END_TEST
