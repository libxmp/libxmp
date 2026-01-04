#include "test.h"

/* Verify LIQ default panning is actually applied in the player.
 * TODO: libxmp gets several things wrong here. */

TEST(test_player_liq_sample_default_pan)
{
	compare_mixer_data(
		"data/liq_smpl_setpan.liq",
		"data/liq_smpl_setpan.data");
}
END_TEST
