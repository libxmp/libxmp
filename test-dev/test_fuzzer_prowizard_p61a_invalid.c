#include "test.h"


TEST(test_fuzzer_prowizard_p61a_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* These inputs caused out-of-bounds reads in the The Player 6.1a
	 * depacker due to a missing note bounds check. All 6 tests triggered
	 * in different places due to multiple note packing cases in this format. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note2");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note2)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note3");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note3)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note4");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note4)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note5.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note5)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_note6");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note6)");

	/* This input caused out-of-bounds reads in the The Player 6.1a
	 * test function due to a faulty check on duplicate samples. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_invalid_sample_dup");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_sample_dup)");

	xmp_free_context(opaque);
}
END_TEST
