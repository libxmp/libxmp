#include "test.h"

/* PTM pattern jump resets break row to 0.
 * TODO: at speeds >=2, this occurs every tick, breaking Bxx Dxx.
 */

TEST(test_effect_pattern_jump_ptm_break)
{
	compare_mixer_data(
		"data/pattern_jump_ptm_break.ptm",
		"data/pattern_jump_ptm_break.data");
}
END_TEST
