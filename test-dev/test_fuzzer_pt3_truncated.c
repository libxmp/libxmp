#include "test.h"

/* This input caused uninitialized reads in the PT 3.6 loader due
 * to missing EOF checks when reading the order list.
 */

TEST(test_fuzzer_pt3_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_pt3_truncated.pt36");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
