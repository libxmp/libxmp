#include "test.h"

/* Verify FunkTracker default panning is actually applied in the player.
 * TODO: libxmp currently gets the "RLO" event wrong.
 */

TEST(test_player_fnk_sample_default_pan)
{
	compare_mixer_data(
		"data/fnk_smpl_setpan.fnk",
		"data/fnk_smpl_setpan.data");
}
END_TEST
