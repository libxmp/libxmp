#include "test.h"

TEST(test_fuzzer_arch_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input high memory consumption in the Archimedes Tracker
	 * loader due to allowing negative pattern counts.
	 */
	ret = xmp_load_module(opaque, "data/f/load_arch_invalid_patterns");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	/* This input caused signed overflows calculating sample loop
	 * lengths. */
	ret = xmp_load_module(opaque, "data/f/load_arch_invalid_sample_loops");
	fail_unless(ret == 0, "module load");

	xmp_free_context(opaque);
}
END_TEST
