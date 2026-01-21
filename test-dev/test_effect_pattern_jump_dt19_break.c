#include "test.h"

/* DTM pattern jump resets break row to 0.
 * Note: prior to Digital Tracker 1.9, Digital Tracker
 * completely ignores the parameter of the pattern break
 * effect?
 */

TEST(test_effect_pattern_jump_dt19_break)
{
	compare_mixer_data(
		"data/pattern_jump_dt19_break.dtm",
		"data/pattern_jump_dt19_break.data");
}
END_TEST
