#include "test.h"

/* Previously, libxmp would not attempt to scan later in a module if
 * the first sequence fails to scan. This is fine for non-marker formats,
 * but S3M and IT can begin with an end marker and contain valid data
 * afterward. These modules are now supported.
 */

TEST(test_fuzzer_play_s3m_marker_sequence)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,		1, -XMP_END },
		{ PLAY_FRAMES,		1, -XMP_END },
		{ PLAY_SET_POSITION,	1, 1 },
		{ PLAY_FRAMES,		4, 0 },
		{ PLAY_END,		0, 0 }
	};
	compare_playback("data/f/play_s3m_marker_sequence.s3m", sequence, 4000, 0, 0);
}
END_TEST
