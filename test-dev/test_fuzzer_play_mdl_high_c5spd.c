#include "test.h"

/* This input caused undefined float-to-integer conversion in the mixer
 * due to extremely high sample C5 speeds.
 */

TEST(test_fuzzer_play_mdl_high_c5spd)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_mdl_high_c5spd.mdl", sequence, 4000, 0, 0);
}
END_TEST
