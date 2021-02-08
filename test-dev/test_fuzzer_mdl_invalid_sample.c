#include "test.h"

/* These inputs caused high amounts of RAM usage and hangs in the MDL
 * loader sample chunk handler for various reasons.
 */

TEST(test_fuzzer_mdl_invalid_sample)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();

	/* Contains an invalid pack type of 3, but the loader would allocate
	 * a giant buffer for it before rejecting it.
	 */
	ret = xmp_load_module(opaque, "data/f/load_mdl_invalid_sample_pack.mdl");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	/* Invalid sample size for a pack type 0 sample. */
	ret = xmp_load_module(opaque, "data/f/load_mdl_invalid_sample_size.mdl");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	/* Invalid sample size for a pack type 2 sample. */
	ret = xmp_load_module(opaque, "data/f/load_mdl_invalid_sample_size2.mdl");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
