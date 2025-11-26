#include "test.h"

/* Synth volume and waveform programs can, in fact, have
 * different speeds. Synth tests 1 and 2 also test this,
 * but by generating very large data files that don't make
 * the problem obvious (probably why this was missed).
 */

TEST(test_player_med_synth_diff_speeds)
{
	compare_med_synth_data(
		"data/med_synth_diff_speeds.med",
		"data/med_synth_diff_speeds.data");
}
END_TEST
