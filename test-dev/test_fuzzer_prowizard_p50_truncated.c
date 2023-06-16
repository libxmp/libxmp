#include "test.h"


TEST(test_fuzzer_prowizard_p50_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* Out of bounds reads due to The Player 5.x test function reading
	 * the list entry before checking whether or not it is in bounds. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p50_truncated");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");

	xmp_free_context(opaque);
}
END_TEST
