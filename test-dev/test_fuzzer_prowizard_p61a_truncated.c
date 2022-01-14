#include "test.h"


TEST(test_fuzzer_prowizard_p61a_truncated)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* These inputs caused out-of-bounds reads in the The Player 6.1a
	 * test function due to faulty PW_REQUEST_DATA calls. */
	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_truncated");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (truncated)");

	ret = xmp_load_module(opaque, "data/f/prowizard_p61a_truncated2");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (truncated2)");

	xmp_free_context(opaque);
}
END_TEST
