#include "test.h"

/* Bxx+Cxx works in either order.
 */

TEST(test_effect_pattern_jump_mpt_break_xm)
{
	compare_mixer_data(
		"data/pattern_jump_mpt_break.xm",
		"data/pattern_jump_mpt_break_xm.data");
}
END_TEST
