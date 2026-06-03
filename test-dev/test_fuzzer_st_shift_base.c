#include "test.h"

/* Negative finetune byte values should not trigger
 * -fsanitize=shift-base warnings/errors.
 */

TEST(test_fuzzer_st_shift_base)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_st_shift_base_finetune.mod");
	fail_unless(ret == -XMP_ERROR_FORMAT, "module load");

	xmp_free_context(opaque);
}
END_TEST
