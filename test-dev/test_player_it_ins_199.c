#include "test.h"

/* IT modules from trackers other than Impulse Tracker support
 * more than 99 samples and instruments. This module contains 200
 * instruments (the MPT 1.16 maximum) and 255 samples; libxmp
 * should be able to play this without issue.
 */

TEST(test_player_it_ins_199)
{
	compare_mixer_data(
		"data/ins199.it.xz",
		"data/ins199.data");
}
END_TEST
