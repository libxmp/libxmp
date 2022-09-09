#include "test.h"

/* General MMD3 test including misc features not covered elsewhere:
 *
 * - 16-bit samples
 * - Sample disable flag
 * - Sample ping-pong loops
 * - Hybrids with waveforms
 * - Octave samples in MMD3
 * - Only Samples and ExtSamples should be transposed down in mix mode
 * - Samples and ExtSamples are transposed the same amount in mix mode
 */

TEST(test_player_med_instruments)
{
	compare_mixer_data(
		"data/instruments.mmd3",
		"data/instruments_mmd3.data");
}
END_TEST
