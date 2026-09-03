#include "test.h"

/* Changing the player mode rescans the module, and previously,
 * changing a non-S3M/IT module beginning with FFh to S3M/IT would
 * cause various issues resulting in an infinite loop trying to
 * locate the next order.
 */

TEST(test_fuzzer_play_mod_bad_marker_sequence)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_SET_PLAYER_MODE,	XMP_MODE_S3M, 0 },
		{ PLAY_FRAMES,		1, -XMP_END },
		{ PLAY_FRAMES,		1, -XMP_END },
		{ PLAY_SET_POSITION,	2, 2 },
		{ PLAY_FRAMES,		4, 0 },
		{ PLAY_END,		0, 0 }
	};
	compare_playback("data/f/play_mod_bad_marker_sequence.mod", sequence, 4000, 0, 0);
}
END_TEST
