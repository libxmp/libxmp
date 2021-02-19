#include "test.h"

/* This input high memory consumption in the Archimedes Tracker
 * loader due to allowing negative pattern counts.
 */

TEST(test_fuzzer_arch_invalid_patterns)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_arch_invalid_patterns");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
