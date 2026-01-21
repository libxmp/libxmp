#include "test.h"

/* Jxx (pattern jump) resets the break row set by Cxx
 * (pattern cut) to 0.
 */

TEST(test_effect_pattern_jump_liq_break_no)
{
	compare_mixer_data(
		"data/pattern_jump_liq_break_no.liq",
		"data/pattern_jump_liq_break_no.data");
}
END_TEST
