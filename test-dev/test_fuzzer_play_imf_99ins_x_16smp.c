#include "test.h"

/* Maximum number of instruments supported by Orpheus with the maximum
 * number of samples per instrument. libxmp should not crash or leak samples.
 *
 * The first module is the result of loading and resaving the XM equivalent
 * of this module with Orpheus (Orpheus appears to have a hard limit of 200
 * samples but will sometimes emit uninitialized samples with nul magic). The
 * second module is a hexedited "theoretical" version of what actually should
 * have been produced by this conversion, which reproduces a crash in
 * affected versions.
 */

TEST(test_fuzzer_play_imf_99ins_x_16smp)
{
	static const struct playback_sequence sequence[] =
	{
		{ PLAY_FRAMES,	2, 0 },
		{ PLAY_END,	0, 0 }
	};
	compare_playback("data/f/play_imf_99ins_x_16smp.imf.xz", sequence, 4000, 0, 0);
	compare_playback("data/f/play_imf_99ins_x_16smp_hexedit.imf.xz", sequence, 4000, 0, 0);
}
END_TEST
