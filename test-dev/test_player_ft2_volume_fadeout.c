#include "test.h"

/* Volume events do not reset fadeout. Likely to counteract other bugs,
 * libxmp was resetting fadeout on some volume events, citing OpenMPT
 * NoteOff.xm and m5v-nine.xm. The former should have actually been
 * resetting fadeout on the keyoff+delay.
 *
 * 00-07: volume does not reset fadeout after fadeout has reached 0.
 * 08-0F: volume does not reset fadeout if the envelope returns 0.
 */

TEST(test_player_ft2_volume_fadeout)
{
	compare_mixer_data(
		"data/ft2_volume_fadeout.xm",
		"data/ft2_volume_fadeout.data");
}
END_TEST
