#include "test.h"

/* In Fasttracker 2, notes without ins# don't reset fadeout.
 * Only ins# without keyoff/K00--or any delayed event--resets fadeout.
 * See test_player_ft2_delay_*.c and test_player_ft2_note_off_*.c
 *
 * Pattern 0: note w/o ins# doesn't reset fadeout without envelope.
 * Pattern 1: note w/o ins# doesn't reset fadeout with envelope.
 * Pattern 2: note w/o ins# doesn't reset release with sustain envelope.
 */

TEST(test_player_ft2_note_no_fadeout_reset)
{
	compare_mixer_data(
		"data/ft2_note_no_fadeout_reset.xm",
		"data/ft2_note_no_fadeout_reset.data");
}
END_TEST
