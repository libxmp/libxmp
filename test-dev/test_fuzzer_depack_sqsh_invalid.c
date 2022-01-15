#include "test.h"


TEST(test_fuzzer_depack_sqsh_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused misaligned reads in the XPK SQSH depacker due
	 * to misusage of type punning combined with misaligned input data. */
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_invalid_alignment.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_alignment)");

	/* This input caused uninitialized reads in the XPK SQSH depacker due
	 * to the checksum being read from uninitialized (but allocated) data. */
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_invalid_checksum.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_checksum)");

	/* This input caused heap corruption in the XPK SQSH depacker
	 * due to a missing bounds check for verbatim blocks. */
	ret = xmp_load_module(opaque, "data/f/depack_sqsh_invalid_verbatim.xpk");
	fail_unless(ret == -XMP_ERROR_DEPACK, "depacking (invalid_verbatim)");

	xmp_free_context(opaque);
}
END_TEST
