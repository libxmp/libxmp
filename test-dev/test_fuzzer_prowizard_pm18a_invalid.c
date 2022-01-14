#include "test.h"


TEST(test_fuzzer_prowizard_pm18a_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Promizer 1.8a
	 * depacker due to a missing check on note reference values. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pm18a_invalid_note");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused signed overflows in the Promizer 1.8a loader due
	 * to not bounding pattern addresses before trying to seek to them. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pm18a_invalid_paddr");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_paddr)");

	xmp_free_context(opaque);
}
END_TEST
