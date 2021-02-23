#include "test.h"

/* This input caused memory leaks in the Quadra Composer loader
 * due to missing sanity checks for duplicate/misordered chunks.
 */

TEST(test_fuzzer_emod_duplicate_chunk)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_emod_duplicate_chunk.emod");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
