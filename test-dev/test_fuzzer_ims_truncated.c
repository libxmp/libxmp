#include "test.h"


TEST(test_fuzzer_ims_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused uninitialized reads in the IMS test function due
	 * to not checking the hio_read return value when reading the sample
	 * offset (located where the 31-instrument MOD magic normally goes).
	 */
	ret = xmp_load_module(opaque, "data/f/load_ims_truncated_magic.ims");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");

	/* Same, except reading the order table. */
	ret = xmp_load_module(opaque, "data/f/load_ims_truncated_header.ims");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module loader");

	xmp_free_context(opaque);
}
END_TEST
