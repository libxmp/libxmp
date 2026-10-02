#include "test.h"

/* Maximum number of instruments supported by FT2 with the maximum
 * number of samples per instrument. libxmp should not crash or leak samples.
 */

TEST(test_fuzzer_play_xm_128ins_x_16smp)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_xm_128ins_x_16smp.xm.xz", sequence, 4000, 0, 0);
}
END_TEST
