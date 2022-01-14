#include "test.h"


TEST(test_fuzzer_prowizard_novotrade_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused the ProWizard loader to load invalid patterns
	 * due to the Novotrade depacker not validating the order list. */
	ret = xmp_load_module(opaque, "data/f/prowizard_novotrade_invalid_order.ntp");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_order)");

	/* This input caused stack corruption in the ProWizard Novotrade
	 * depacker due to an out of bounds order count. */
	ret = xmp_load_module(opaque, "data/f/prowizard_novotrade_invalid_order_count.ntp");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_order_count)");

	/* This input caused stack corruption in the ProWizard Novotrade
	 * depacker due to an out of bounds pattern count. */
	ret = xmp_load_module(opaque, "data/f/prowizard_novotrade_invalid_pattern_count.ntp");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_count)");

	xmp_free_context(opaque);
}
END_TEST
