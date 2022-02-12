#include "test.h"

/* Some of the samples in this IT have bad sustain loop endpoints,
 * which due to a regression were able to crash the mixer.
 */

TEST(test_fuzzer_play_it_bad_sustain)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	10, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_it_bad_sustain.it", sequence, 4000, 0, 0);
}
END_TEST
