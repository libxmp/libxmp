#include "test.h"

/* The MTM tempo effect has the weird property of resetting the
 * speed to 6 if the tempo is set, and resetting the tempo to 125
 * if the speed is set. For a real module that relies on this,
 * see absolve.mtm orders 22 and 23.
 *
 * A lot of MTM modules rely on DMP's timing instead, which
 * emulates Protracker. When both a speed and tempo effect are
 * found on the same row, this timing generally should be used.
 */

TEST(test_player_mtm_tempo)
{
	compare_mixer_data(
		"data/TEMPO.MTM",
		"data/tempo_mtm.data");
	compare_mixer_data(
		"data/TEMPO2.MTM",
		"data/tempo2_mtm.data");
}
END_TEST
