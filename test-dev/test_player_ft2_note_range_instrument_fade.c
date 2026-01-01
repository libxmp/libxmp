#include "test.h"

/* While out-of-range notes prevent updating the active sample, they
 * don't prevent updating other instrument properties like envelopes,
 * fadeout rate, and vibrato. This module tests fadeout rate only.
 *
 * 00-03: normal fadeout for instrument 2.
 * 04-07: >A#9 - instrument 2 sample continues with instrument 1 fadeout.
 * 08-0B: B-(-1) - instrument 1 C#0 sample plays with instrument 1 fadeout.
 * 0C-0F: A#(-1) - instrument 2 sample continues with instrument 1 fadeout.
 */

TEST(test_player_ft2_note_range_instrument_fade)
{
	compare_mixer_data(
		"data/ft2_note_range_instrument_fade.xm",
		"data/ft2_note_range_instrument_fade.data");
}
END_TEST
