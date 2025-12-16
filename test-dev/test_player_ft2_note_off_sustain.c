#include "test.h"

/* Instrument numbers reset sustain release unless they
 * are accompanied by a key off (or K00 without toneporta).
 * These also prevent the envelope positions from being reset.
 *
 * Active invalid instruments do not block clearing
 * sustain release, unlike with envelope position reset.
 *
 * 00-07: lone instrument number resets envelopes/release.
 * 08-0F: delayed keyoff + instrument number resets envelopes/release.
 * 10-15: keyoff + instrument number does not reset envelopes/release.
 * 16-1B: K00 + instrument number does not reset envelopes/release.
 * 1C-23: Envelope positions are not reset (or advanced) while an
 *        invalid instrument is active, but release is reset on rows
 *        1E and 1F. The envelope should continue where it left off
 *        starting at row 20 and hold at the sustain point.
 */

TEST(test_player_ft2_note_off_sustain)
{
	compare_mixer_data(
		"data/ft2_note_off_sustain.xm",
		"data/ft2_note_off_sustain.data");
}
END_TEST
