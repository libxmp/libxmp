#include "test.h"

/* This FAR underflows the tempo in old tempo mode, one of the several
 * places in far_extras.c that used to potentially left shift a negative
 * tempo. The new handling doesn't do this, so this shouldn't crash with
 * -fsanitize=shift-base -fno-sanitize-recover=all.
 *
 * This needs to play a lot of frames (5 rows * 32 native frames per row)
 * for futureproofing, though libxmp only executes 16 frames per row currently.
 */

TEST(test_fuzzer_play_far_old_tempo_mode_underflow)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	5 * 32, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_far_old_tempo_mode_underflow.far", sequence, 4000, 0, 0);
}
END_TEST
