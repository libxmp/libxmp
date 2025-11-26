#include "test.h"

/* Test MED hold/decay plus no-delay retrigger (1F0x).
 * This module relies on tick 0 slides and works in every version of
 * OctaMED between OctaMED 3.00 and MED Soundstudio 2.
 *
 * Left and right should play the same for the entire module.
 *
 * Every retrigger should add the CURRENT TICK NUMBER to the hold count.
 * Presumably, this was intended to be the retrigger delay, but someone
 * decided it was okay to have retriggers after the first break (or forgot?).
 *
 * Pattern 0: tests each value of x from 1 to F for a fixed row length.
 * Pattern 1: tests 1F05 specifically in increasing numbers of retriggers.
 */

TEST(test_player_med_hold_1f0x)
{
	compare_mixer_data(
		"data/med_hold_1f0x.med",
		"data/med_hold_1f0x.data");
}
END_TEST
