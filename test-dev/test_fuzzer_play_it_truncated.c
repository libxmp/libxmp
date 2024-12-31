#include "test.h"

/* Test player bugs caused by truncated samples.
 */

TEST(test_fuzzer_play_it_truncated)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};

	/* This input caused uninitialized reads in the mixer due to the
	 * sample truncation handling allowing extra non-frame-aligned bytes
	 * at the end of a sample, which would never be initialized properly.
	 */
	compare_playback("data/f/play_it_truncated_sample.it",
		sequence, 4000, 0, 0);
}
END_TEST
