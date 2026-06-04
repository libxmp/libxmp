#include "test.h"


TEST(test_fuzzer_play_it_high_transpose)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	24, 0 },
		{ PLAY_END,	0, 0 }
	};

	/* This IT has a sample with the maximum possible transpose value
	 * 9999999 (without hex editing). In linear frequency mode, this
	 * could cause libxmp to calculate a note value of 241, which
	 * caused a leftshift of a negative value in libxmp_period_to_bend.
	 * This should play without causing any problems.
	 */
	compare_playback("data/f/play_it_high_transpose.it", sequence, 4000, 0, 0);
}
END_TEST
