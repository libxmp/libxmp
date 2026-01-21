#include "test.h"

/* MTM pattern jump resets break row to 0.
 */

TEST(test_effect_pattern_jump_mtm_break)
{
	compare_mixer_data(
		"data/pattern_jump_mtm_break.mtm",
		"data/pattern_jump_mtm_break.data");
}
END_TEST
