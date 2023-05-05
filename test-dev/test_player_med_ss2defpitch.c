#include "test.h"

/* MED Soundstudio 2 added default note events, which play the
 * note stored in the default_pitch InstrExt field. This event
 * is coded as note value 0x01, so it only works in MMD3--non
 * mix mode formats use note 0x01 for C-1.
 *
 * In prior versions of OctaMED, this field was only used to
 * supply a note number for the default note key 'F'. It had
 * no effect in the player.
 */

TEST(test_player_med_ss2defpitch)
{
	compare_mixer_data(
		"data/ss2defpitch.med",
		"data/ss2defpitch.data");
}
END_TEST
