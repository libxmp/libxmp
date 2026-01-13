#include "test.h"

/* Combining Mx+3xx/5xy results in both effects advancing toneporta.
 * rst SoundTracker uses shared toneporta memory like FT2 and updates
 * the toneporta parameters every tick (MF 300 and M0 3F0 are equivalent).
 * TODO: rows 1C/1D don't play correctly because libxmp doesn't support
 * processing toneporta parameters every tick.
 */

TEST(test_player_rstst_double_toneporta)
{
	compare_mixer_data(
		"data/rstst_double_toneporta.xm",
		"data/rstst_double_toneporta.data");
}
END_TEST
