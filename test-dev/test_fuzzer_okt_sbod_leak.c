#include "test.h"

/* This input caused leaks in the Oktalyzer loader due to
 * loading unused samples over existing samples.
 */

TEST(test_fuzzer_okt_sbod_leak)
{
	xmp_context opaque;
	struct xmp_module_info info;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_okt_sbod_leak.okt");
	fail_unless(ret == -XMP_ERROR_LOAD, "module load");

	xmp_free_context(opaque);
}
END_TEST
