#include "test.h"

TEST(test_fuzzer_play_rtm_autovib)
{
	/* This RTM has two instruments that use autovibrato depths
	 * out-of-range in Real Tracker, but which play correctly.
	 * One instrument has a positive rate and the other has a
	 * negative rate (another out-of-range, but working, setting),
	 * and they are played offset so that both channels should
	 * be synchronized.
	 *
	 * This previously caused out-of-bounds array accesses in lfo.c.
	 */
	compare_mixer_data(
		"data/f/play_rtm_autovib_oob_depth_rate.rtm",
		"data/f/play_rtm_autovib_oob_depth_rate.data");
}
END_TEST
