#include "test.h"

/*
 The interpretation of this scenario highly differs between various replayers.
 If the sample number next to a portamento effect differs from the previous
 number, the new sample's default volume should be applied and

 o the old sample should be played until reaching its end or loop end
   (ProTracker 1/2). If the sample loops, the new sample should be activated
   after the loop ended.

 o the old sample should keep playing (various other players)

 OpenMPT implements the second option in normal playback mode and the
 first option in ProTracker 1/2 mode.

*/

TEST(test_openmpt_mod_portasmpchange)
{
	/* Test generic MOD player behavior. */
	compare_mixer_data_player_mode(
		"openmpt/mod/PortaSmpChange.mod",
		"openmpt/mod/PortaSmpChange.data",
		XMP_MODE_MOD);

	/* Test Protracker 1/2 behavior. */
	compare_mixer_data(
		"openmpt/mod/PortaSmpChange.mod",
		"openmpt/mod/PortaSmpChange_PT.data");
}
END_TEST
