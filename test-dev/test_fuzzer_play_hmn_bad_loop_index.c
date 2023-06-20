#include "test.h"

/* This input caused out-of-bounds reads in the
 * His Master's Noise Mupp instrument interpreter.
 */

TEST(test_fuzzer_play_hmn_bad_loop_index)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	3, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_hmn_bad_loop_index.mod", sequence, 4000, 0, 0);
}
END_TEST
