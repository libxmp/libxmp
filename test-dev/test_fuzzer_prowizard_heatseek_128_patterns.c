#include "test.h"

/* This input caused extremely slow loads in the Heatseeker test function
 * due to frequent PW_REQUEST_DATA usage for very small amounts of data.
 *
 * This required a very specific situation to trigger: ProWizard must
 * reach the Heatseeker test function without loading any extra data beyond
 * the initial read. This means the Module Protector no-ID test function
 * needs to fail BEFORE it checks patterns, which for this module depended
 * on the following property: the MP no-ID test function fails if the
 * pattern count is more than 3 patterns greater than the module length,
 * but the Heatseeker test function does not.
 */

TEST(test_fuzzer_prowizard_heatseek_128_patterns)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/prowizard_heatseek_128_patterns.xz");
	fail_unless(ret == 0, "module load");

	xmp_free_context(opaque);
}
END_TEST
