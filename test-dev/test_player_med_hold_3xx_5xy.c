#include "test.h"

/* Test MED hold/decay plus toneporta (including MMD1 5xy).
 * This module relies on tick 0 slides and works in every version of
 * OctaMED between OctaMED 3.00 and MED SoundStudio 2.
 *
 * Left channel is an instrument with hold 1 decay 15;
 * the right channel does not use hold/decay (except in rows 6-11)
 * and sounds identical to the left channel in MED/OctaMED.
 *
 * Row 00-05: note+3xx on line after initial line sustains (as 3xx test).
 * Row 06-11: note+5xy does NOT sustain, does NOT initialize toneporta.
 *            TODO: libxmp gets the former correct but the later wrong.
 *            Also note that the decay hits 0 before the end of row, so
 *            the 510 actually should make the note fade in slightly after
 *            the decay terminates.
 * Row 12-17: lone 5xy after note+3xx does not sustain, still volslides.
 * Row 18-25: no-note+instrument sustain works with 5xy.
 */

TEST(test_player_med_hold_3xx_5xy)
{
	compare_mixer_data(
		"data/med_hold_3xx_5xy.med",
		"data/med_hold_3xx_5xy.data");
}
END_TEST
