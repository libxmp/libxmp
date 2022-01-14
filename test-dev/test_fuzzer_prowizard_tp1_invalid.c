#include "test.h"


TEST(test_fuzzer_prowizard_tp1_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input crashed the ProWizard TP1 depacker due to
	 * a missing module length bounds check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_tp1_invalid_length.xz");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid length)");

	/* This input caused signed overflows in the TP1 loader due to
	 * strange pattern address calculation being done in signed ints
	 * instead of unsigned ints. */
	ret = xmp_load_module(opaque, "data/f/prowizard_tp1_invalid_paddr");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid paddr)");

	xmp_free_context(opaque);
}
END_TEST
