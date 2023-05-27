#include "test.h"


TEST(test_fuzzer_prowizard_p50_127_patterns)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();

	/* Pattern loader jank caused this valid module to write well out of
	 * bounds of the pattern array. (Caused by runs.) */
	ret = xmp_load_module(opaque, "data/f/prowizard_p50_127_patterns");
	fail_unless(ret == 0, "module load");

	/* Pattern loader jank caused this valid module to write well out of
	 * bounds of the pattern array. (Caused by backreferences.)*/
	ret = xmp_load_module(opaque, "data/f/prowizard_p50_127_patterns2");
	fail_unless(ret == 0, "module load");

	xmp_free_context(opaque);
}
END_TEST
