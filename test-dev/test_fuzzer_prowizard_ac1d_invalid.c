#include "test.h"


TEST(test_fuzzer_prowizard_ac1d_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the ac1d depacker
	 * due to a missing note bounds check. */
        ret = xmp_load_module(opaque, "data/f/prowizard_ac1d_invalid_note");
        fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused integer overflows in the ac1d depacker due to
	 * computing (unused) pattern sizes from invalid pattern addresses. */
	ret = xmp_load_module(opaque, "data/f/prowizard_ac1d_invalid_paddr");
	fail_unless(ret == 0, "module load (invalid_paddr)");

	xmp_free_context(opaque);
}
END_TEST
