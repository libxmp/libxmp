#include "test.h"
#include "../src/effects.h"

/* Unlike most XM-clone trackers, Fasttracker 2 allows the Lxx
 * effect to entirely skip the envelope loop and sustain points.
 * This module tests a variety of different cases of this. See
 * the Modplug comment text in the test XM for more info.
 */
TEST(test_player_xm_envelope_set_pos)
{
	compare_mixer_data(
		"data/lxx_after_loop.xm",
		"data/lxx_after_loop.data");
}
END_TEST
