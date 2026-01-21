#include "test.h"

/* Bxx+Cxx works in either order.
 */

TEST(test_effect_pattern_jump_it_break)
{
	compare_mixer_data(
		"data/pattern_jump_it_break.it",
		"data/pattern_jump_it_break.data");
}
END_TEST
