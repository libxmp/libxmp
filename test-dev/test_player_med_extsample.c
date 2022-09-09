#include "test.h"

/* ExtSamples in MMD2 mode have two extra octaves of range and are
 * transposed down two octaves. Five different pitches of notes
 * should play; the sixth wraps down an octave like with normal samples.
 */

TEST(test_player_med_extsample)
{
	compare_mixer_data(
		"data/extsample.mmd2",
		"data/extsample.data");
}
END_TEST
