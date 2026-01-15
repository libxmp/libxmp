#include "test.h"

/* This module should play the first pattern backward and
 * then continue to the second pattern. Each row should increase
 * the play time by 50ms (and never reset the play time to the
 * start time of the first pattern).
 */

TEST(test_effect_pattern_jump_it_samepos)
{
	compare_mixer_data(
		"data/pattern_jump_it_samepos.it",
		"data/pattern_jump_it_samepos.data");
}
END_TEST
