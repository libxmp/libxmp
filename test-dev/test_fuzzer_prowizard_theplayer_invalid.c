#include "test.h"


TEST(test_fuzzer_prowizard_theplayer_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the The Player
	 * depacker due to a missing note bounds check. */
	ret = xmp_load_module(opaque, "data/f/prowizard_theplayer_invalid_note.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused out-of-bounds reads in the The Player 5.x/6.0x
	 * test function due to a faulty check on duplicate samples. */
	ret = xmp_load_module(opaque, "data/f/prowizard_theplayer_invalid_sample_dup");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_sample_dup)");

	xmp_free_context(opaque);
}
END_TEST
