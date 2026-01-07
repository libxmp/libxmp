#include "test.h"

/* Combining Gx+Gxx/Lxy results in both effects advancing toneporta.
 * Modplug Tracker uses shared toneporta memory like IT and updates
 * the toneporta parameters every tick (GF G00 and G0 GFF are equivalent).
 * TODO: there are numerous errors here due to libxmp incorrectly applying
 * (wrong) Impulse Tracker semantics.
 */

TEST(test_player_mpt_it_double_toneporta)
{
	compare_mixer_data(
		"data/mpt_it_double_toneporta.it",
		"data/mpt_it_double_toneporta.data");
}
END_TEST
