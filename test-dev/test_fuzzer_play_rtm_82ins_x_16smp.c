#include "test.h"

/* Real Tracker 2 supports loading an XM with 128 instruments (and possibly
 * more natively) but eventually starts failing to allocate memory for samples.
 * 82x16 is the maximum I could save. libxmp should not crash or leak samples.
 */

TEST(test_fuzzer_play_rtm_82ins_x_16smp)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_rtm_82ins_x_16smp.rtm.xz", sequence, 4000, 0, 0);
}
END_TEST
