#include "test.h"

/* Versions prior to OctaMED 3.00 do not set default
 * volume on hold events. Instruments without decay
 * set should also not decay after the end of a
 * hold event. */

TEST(test_player_med_hold_no_default_volume)
{
	compare_mixer_data(
		"data/med_hold_no_default_volume.med",
		"data/med_hold_no_default_volume.data");
}
END_TEST
