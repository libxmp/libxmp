#include "test.h"

/* The sample in this module has junk loop values that get ignored
 * by the sample loader because the sample is unloadable. This sample
 * should be rejected by the player, too.
 */

TEST(test_fuzzer_play_it_bad_loop)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	10, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_it_bad_loop.it", sequence, 4000, 0, 0);
}
END_TEST
