#include "test.h"

/* Tone portamento should cancel the NNA for the current note
 * even if the current note is a sample change (and thus won't
 * actually perform the tone portamento).
 */

TEST(test_player_it_portamento_nna_sample)
{
	compare_mixer_data(
		"data/portamento_nna_sample.it",
		"data/portamento_nna_sample.data");
}
END_TEST
