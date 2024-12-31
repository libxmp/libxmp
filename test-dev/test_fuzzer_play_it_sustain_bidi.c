#include "test.h"

/* Test player bugs caused by sustain release combined with
 * bidirectional loops, which unfortunately keep showing up.
 */

TEST(test_fuzzer_play_it_sustain_bidi)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	static const struct playback_sequence sequence_long[] =
	{
		{ PLAY_FRAMES,	20, 0 },
		{ PLAY_END,	0, 0 }
	};

	/* This module has a sample with a bidi sustain loop and a non-bidi
	 * regular loop. This was able to cause crashes in libxmp due to its
	 * bad bidi and sustain support prior to overhaul.
	 */
	compare_playback("data/f/play_it_sustain_bidi.it",
		sequence, 4000, 0, 0);

	/* This module has a sample with a bidi sustain loop and a non-bidi
	 * regular loop. It encounters an edge case, where sustain is released
	 * when the position is negative but the loop hasn't been reflected
	 * around its starting point yet.
	 */
	compare_playback("data/f/play_it_sustain_bidi2.it",
		sequence_long, 4000, 0, 0);

	/* This module has a sample with a bidi sustain loop and an invalid
	 * regular loop. The high frequency it plays at can trigger the mixer
	 * loop's hang detection and leave its position at a negative value,
	 * at which point sustain release can be used to cause trouble.
	 */
	compare_playback("data/f/play_it_sustain_bidi3.it",
		sequence_long, 4000, 0, 0);

	/* This module encountered a similar issue to the sustain/bidi
	 * bugs--the position advances far past the end of the sample at
	 * very low sample rates, which the effect S9F Reverse can then
	 * cause a crash at.
	 */
	compare_playback("data/f/play_it_reverse_past_end.it",
		sequence_long, 4000, 0, 0);
}
END_TEST
