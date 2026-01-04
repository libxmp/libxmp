#include "test.h"

/* Verify IMF default panning is actually applied in the player.
 * TODO: ins# without note only temporarily sets panning,
 * libxmp understandably gets this wrong.
 */

TEST(test_player_imf_sample_default_pan)
{
	compare_mixer_data(
		"data/imf_smpl_setpan.imf",
		"data/imf_smpl_setpan.data");
}
END_TEST
