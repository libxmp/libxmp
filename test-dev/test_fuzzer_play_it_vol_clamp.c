#include "test.h"

/* This IT contains out-of-bounds global volume values in the sample
 * and instrument, as well as out-of-bounds envelope volume and channel
 * volume. libxmp was clamping the latter two before but not the former.
 * In conjunction with NNA spam which is hard to analyze, these were
 * causing signed integer overflows in the mixer.
 *
 * Since the UB is hard to reproduce in a controlled manner, just make
 * sure the mixer is seeing a normal volume level on this note.
 */

TEST(test_fuzzer_play_it_vol_clamp)
{
	compare_mixer_data(
		"data/f/play_it_vol_clamp.it",
		"data/f/play_it_vol_clamp.data");
}
END_TEST
