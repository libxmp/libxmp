#include "test.h"

/* ST2 always breaks to row 0(!).
 * TODO: channel 2 should mute on row 1, but does not,
 * because libxmp doesn't implement ST2's delayed pattern
 * jump behavior.
 */

TEST(test_effect_pattern_jump_st2_break)
{
	compare_mixer_data(
		"data/pattern_jump_st2_break.stm",
		"data/pattern_jump_st2_break.data");
}
END_TEST
