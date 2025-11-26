#include "test.h"

/* Test MED hold/decay plus toneporta.
 * This module relies on tick 0 slides and works in every version of
 * MED/OctaMED between MED 3.00 and MED SoundStudio 2.
 *
 * Left channel is an instrument with hold 1 decay 15;
 * the right channel does not use hold/decay and sounds
 * identical to the left channel in MED/OctaMED.
 *
 * Row 00-05: note+toneporta on line after initial line sustains.
 * Row 06-11: further lone toneportas afterward do not sustain.
 * Row 12-17: note+toneporta sustain does not need an instrument number.
 * Row 18-25: no-note+instrument sustain works with toneporta.
 * Row 26-33: note+toneporta sustain works after no-note+instrument sustain.
 */

TEST(test_player_med_hold_3xx)
{
	compare_mixer_data(
		"data/med_hold_3xx.med",
		"data/med_hold_3xx.data");
}
END_TEST
