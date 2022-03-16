#include "test.h"

/* Offset + reverse could result in the sample position being placed
 * beyond the end of the sample, causing crashes.
 */

TEST(test_fuzzer_play_it_offset_reverse)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	4, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_it_offset_reverse.it", sequence, 4000, 0, 0);
}
END_TEST
