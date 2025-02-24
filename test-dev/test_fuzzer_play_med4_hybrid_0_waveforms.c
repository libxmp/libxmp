#include "test.h"

/* Hybrids with 0 waveforms should not attempt to load their sample.
 */

TEST(test_fuzzer_play_med4_hybrid_0_waveforms)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_med4_hybrid_0_wforms.med", sequence, 4000, 0, 0);
}
END_TEST
