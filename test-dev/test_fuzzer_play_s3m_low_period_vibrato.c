#include "test.h"

/* This S3M produces a very low note periods that could underflow
 * libxmp's period calculation below 0 in some cases, causing invalid
 * float to integer conversions.
 */

TEST(test_fuzzer_play_s3m_low_period_vibrato)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	8, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_s3m_low_period_vibrato.s3m", sequence, 4000, 0, 0);
}
END_TEST
