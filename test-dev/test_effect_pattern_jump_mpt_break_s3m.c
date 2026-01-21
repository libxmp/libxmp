#include "test.h"

/* Bxx+Cxx works in either order.
 */

TEST(test_effect_pattern_jump_mpt_break_s3m)
{
	compare_mixer_data(
		"data/pattern_jump_mpt_break.s3m",
		"data/pattern_jump_mpt_break_s3m.data");
}
END_TEST
