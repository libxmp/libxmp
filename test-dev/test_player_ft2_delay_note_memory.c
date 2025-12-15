#include "test.h"

/* Fasttracker 2 note delay has a strange feature resembling "note memory"
 * or "retrigger" where, when no note is set on a delayed row, Fasttracker 2
 * will pretend there's a note set anyway, reusing the last valid note.
 * This note will then use the instrument number beside it, or instrument
 * memory if there isn't one, to play a new note. This causes samples to
 * restart from the beginning and activates new instruments (including
 * invalid instruments). However, if there is no instrument number in the
 * delayed row, this will NOT apply instrument defaults.
 *
 * No envelope (for envelope versions, add 1C to the row numbers):
 *
 * 00-07: normal non-delayed instrument numbers. They don't change instruments.
 * 0A   : no note/no ins# + volume 40h + delay -> "retrig".
 * 0C   : no note/ins# + delay -> retrig, new instrument, apply its defaults.
 * 0F   : this delayed row sets the active instrument to an invalid instrument!
 * 11   : this delayed row reactivates the channel.
 * 12   : retrigger, but do not apply defaults.
 * 15-16: keyoff -> do not reuse the old note, change instrument, or retrigger.
 * 17   : new note -> do not reuse the old note.
 * 19   : ED0, as always, does nothing.
 */

TEST(test_player_ft2_delay_note_memory)
{
	compare_mixer_data(
		"data/ft2_delay_note_memory.xm",
		"data/ft2_delay_note_memory.data");
}
END_TEST
