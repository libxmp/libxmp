#include "test.h"

/* OctaMED 3.00 and up set the default volume on
 * a hold event despite not changing the instrument.
 * Instruments without decay set should also not
 * decay after the end of a hold event.
 *
 * Something weird happens if the note has hold/decay
 * and the final instrument number in the hold symbol chain
 * is different from the initial instrument--this is not
 * tested here. */

TEST(test_player_med_hold_default_volume)
{
	compare_mixer_data(
		"data/med_hold_default_volume.med",
		"data/med_hold_default_volume.data");
}
END_TEST
