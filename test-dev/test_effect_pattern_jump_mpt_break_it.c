#include "test.h"

/* Bxx+Cxx works in either order.
 */

TEST(test_effect_pattern_jump_mpt_break_it)
{
	compare_mixer_data(
		"data/pattern_jump_mpt_break.it",
		"data/pattern_jump_mpt_break_it.data");
}
END_TEST
