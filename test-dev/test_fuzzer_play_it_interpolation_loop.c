#include "test.h"

/* This module caused reads from the sample left redzone
 * due to the interpolation loop wraparound initialization
 * copying twice as many samples as necessary.
 */

TEST(test_fuzzer_play_it_interpolation_loop)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_it_interpolation_loop.it", sequence, 4000, 0, 0);
}
END_TEST
