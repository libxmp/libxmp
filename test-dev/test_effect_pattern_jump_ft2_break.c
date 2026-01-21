#include "test.h"

/* XM pattern jump resets break row to 0.
 */

TEST(test_effect_pattern_jump_ft2_break)
{
	compare_mixer_data(
		"data/pattern_jump_ft2_break.xm",
		"data/pattern_jump_ft2_break.data");
}
END_TEST
