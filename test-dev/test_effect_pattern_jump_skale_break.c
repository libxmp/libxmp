#include "test.h"

/* In Skale Tracker, Bxx+Dxx works in either order for some reason.
 */

TEST(test_effect_pattern_jump_skale_break)
{
	compare_mixer_data(
		"data/pattern_jump_skale_break.xm",
		"data/pattern_jump_skale_break.data");
}
END_TEST
