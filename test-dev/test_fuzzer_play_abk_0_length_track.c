#include "test.h"


TEST(test_fuzzer_play_abk_0_length_track)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	384, 0 },
		{ PLAY_END,	0, 0 }
	};

	/* This ABK contains a track hex edited to emit a tempo command, followed
	 * immediately by an end-of-track marker. It's not clear what this would
	 * actually do in official software, but in libxmp, the loader would
	 * write a break command to (length - 1), which corrupted the track length. */
	compare_playback("data/f/play_abk_0_length_track.abk", sequence, 4000, 0, 0);
}
END_TEST
