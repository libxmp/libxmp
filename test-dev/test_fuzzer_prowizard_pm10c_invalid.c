#include "test.h"


TEST(test_fuzzer_prowizard_pm10c_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused an out-of-bounds read in the Promizer 1.0c
	 * depacker due to a missing check on note reference values. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pm10c_invalid_note.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_note)");

	/* This input caused an out-of-bounds read in the Promizer 1.0c
	 * depacker due to failure to validate sample and finetune values. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pm10c_invalid_pattern.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_pattern)");

	/* This input caused an out-of-bounds read in the Promizer 1.0c
	 * depacker due to failure to validate sample and finetune values.
	 * That should never actually be reached since this also tries to
	 * read far more pattern data than exists in the file. */
	ret = xmp_load_module(opaque, "data/f/prowizard_pm10c_invalid_psize.xz");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load (invalid_psize)");

	xmp_free_context(opaque);
}
END_TEST
