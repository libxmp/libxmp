#include "test.h"


TEST(test_fuzzer_umx_invalid)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* This input caused slow loads in the UMX loader due to containing a
	 * large invalid number of type names. */
	ret = xmp_load_module(opaque, "data/f/load_umx_invalid_names.umx.xz");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_names)");

	/* This input caused UMRs due to not checking an hio_read
	 * return value. */
	ret = xmp_load_module(opaque, "data/f/load_umx_invalid_objname.umx");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load (invalid_objname)");

	xmp_free_context(opaque);
}
END_TEST
