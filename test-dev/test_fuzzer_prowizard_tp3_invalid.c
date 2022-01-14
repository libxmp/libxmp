#include "test.h"


TEST(test_fuzzer_prowizard_tp3_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input crashed the ProWizard TP2/3 depacker due to
	 * a missing patterns bound check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_tp3_invalid_pattern_count");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern_count)");

	/* This input crashed the ProWizard TP2/3 depacker due to
	 * a samples count >31. */
	ret = xmp_load_module(opaque, "data/f/prowizard_tp3_invalid_sample_count");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_sample_count)");

	xmp_free_context(opaque);
}
END_TEST
