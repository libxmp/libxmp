#include "test.h"

/* Combining Mx+3xx/5xy results in both effects advancing toneporta.
 * MadTracker 2 uses shared toneporta memory like FT2 and updates
 * the toneporta parameters every tick AFTER the start tick. This causes
 * something like M0 315 to use prior memory for M0 on the first toneporta
 * update and then use 15h for M0 on every other update that row.
 * TODO: rows 1C/1D don't play correctly because libxmp doesn't support
 * the above broken behavior.
 */

TEST(test_player_mt2_xm_double_toneporta)
{
	compare_mixer_data(
		"data/mt2_xm_double_toneporta.xm",
		"data/mt2_xm_double_toneporta.data");
}
END_TEST
