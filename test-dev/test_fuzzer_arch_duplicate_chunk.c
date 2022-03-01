#include "test.h"


TEST(test_fuzzer_arch_duplicate_chunk)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused out-of-bounds reads and frees past the end of
	 * the patterns array due to loading a garbage pattern count before
	 * validating it after loading prior PNUM and PATT chunks. */
	ret = xmp_load_module(opaque, "data/f/load_arch_duplicate_pnum");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
