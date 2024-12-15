#include "test.h"


TEST(test_fuzzer_play_it_adpcm_bound)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	64, 0 },
		{ PLAY_END,	0, 0 }
	};

	/* This IT was hexedited to contain 32 very long ADPCM samples that
	 * can't possibly fit in the file. Instead of allocating a >1GiB
	 * buffer and then failing to load, libxmp now gracefully truncates
	 * these samples like every other sample.
	 */
	compare_playback("data/f/play_it_adpcm_bound.it", sequence, 4000, 0, 0);
}
END_TEST
