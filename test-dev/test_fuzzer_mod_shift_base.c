#include "test.h"

/* Negative finetune byte values should not trigger
 * -fsanitize=shift-base warnings/errors.
 */

TEST(test_fuzzer_mod_shift_base)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mod_shift_base_finetune.mod");
	fail_unless(ret == 0, "module load");

	xmp_free_context(opaque);
}
END_TEST
