#include "test.h"

/* Jxx (pattern jump) resets the break row set by Cxx
 * (pattern cut) to 0.
 */

TEST(test_effect_pattern_jump_liq_break)
{
	compare_mixer_data(
		"data/pattern_jump_liq_break.liq",
		"data/pattern_jump_liq_break.data");
}
END_TEST
