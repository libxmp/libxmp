#include "test.h"

/* The MTM tempo effect has the weird property of resetting the
 * speed to 6 if the tempo is set, and resetting the tempo to 125
 * if the speed is set. For a real module that relies on this,
 * see absolve.mtm orders 22 and 23. */

TEST(test_player_mtm_tempo)
{
	compare_mixer_data(
		"data/TEMPO.MTM",
		"data/tempo_mtm.data");
}
END_TEST
