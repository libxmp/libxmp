#include "test.h"


TEST(test_fuzzer_prowizard_starpack_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Startrekker Packer
	 * depacker due to 1) test_starpack not checking for junk order data;
	 * 2) depack_starpack using the wrong value for the pattern count; and
	 * 3) depack_starpack allowing "unused" patterns to overflow the pattern
	 * addresses buffer. */
	ret = xmp_load_module(opaque, "data/f/prowizard_starpack_invalid_order");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");

	xmp_free_context(opaque);
}
END_TEST
