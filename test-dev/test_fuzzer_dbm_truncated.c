#include "test.h"

/* This input caused hangs in the DBM loader due to not checking
 * for EOF when loading pattern data.
 */

TEST(test_fuzzer_dbm_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_dbm_truncated.dbm");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
