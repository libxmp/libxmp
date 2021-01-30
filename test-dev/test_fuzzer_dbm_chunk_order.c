#include "test.h"

/* This input crashed libxmp due to the DBM loader allowing a PATT chunk
 * to load before the pattern count was loaded from the INFO chunk.
 */

TEST(test_fuzzer_dbm_chunk_order)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_dbm_chunk_order.dbm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
