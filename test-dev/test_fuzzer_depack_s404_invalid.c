#include "test.h"


TEST(test_fuzzer_depack_s404_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* Make sure that StoneCracker 4.04 inputs with an impossibly high
	 * compression ratio get rejected before allocating a lot of RAM. */
	ret = xmp_load_module(opaque, "data/f/depack_s404_invalid_length");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_length)");

	/* This input caused shifts by invalid exponents in the S404 depacker. */
	ret = xmp_load_module(opaque, "data/f/depack_s404_invalid_shift");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_shift)");

	xmp_free_context(opaque);
}
END_TEST
