#include "test.h"

/* This XM has a volume envelope with extreme values -32768 and 32767.
 * These should be clamped to the range [0,64]. Not doing this causes
 * weird and undefined behavior (e.g. integer overflows found by UBSan).
 */

TEST(test_fuzzer_play_xm_vol_env_clamp)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	8, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_xm_vol_env_clamp.xm", sequence, 4000, 0, 0);
}
END_TEST
