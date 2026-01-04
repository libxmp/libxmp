#include "test.h"

/* Verify DBM default panning is actually applied in the player. */

TEST(test_player_dbm_sample_default_pan)
{
	compare_mixer_data(
		"data/dbm_smpl_setpan.dbm",
		"data/dbm_smpl_setpan.data");
}
END_TEST
