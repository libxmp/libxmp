#include "test.h"

/* IT's filter mixers clamp the filter output and libxmp wasn't doing this,
 * causing clipping and undefined behavior in the filter mixers.
 */

TEST(test_fuzzer_play_it_extreme_filter)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	8, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_it_extreme_filter.it", sequence, 4000, 0, 0);
}
END_TEST
