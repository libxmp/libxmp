#include "test.h"

/* Test MED hold/decay plus pattern delay (1Exx).
 * This module relies on tick 0 slides and works in every version of
 * OctaMED between OctaMED 3.00 and OctaMED Soundstudio 1.
 *
 * Left and right should play the same for the entire module.
 *
 * Row 00-07: decay occurs normally on the pattern delay ticks.
 * Row 08-19: hold counts down normally on the pattern delay ticks.
 * Row 20-25: pattern delay on initial note does not affect hold count.
 * Row 26-35: pattern delay on sustained rows (rows preceding a hold symbol)
 *            does not extend the sustain; once ticks>=speed, hold countdown
 *            begins! (Soundstudio 2 changed this to the more sensible
 *            behavior: sustain lasts the whole row.)
 */

TEST(test_player_med_hold_1exx)
{
	compare_mixer_data(
		"data/med_hold_1exx.med",
		"data/med_hold_1exx.data");
}
END_TEST
