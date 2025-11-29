#include "test.h"

/* Test MED hold/decay plus retrigger (FF1/FF3) and note delay (FF2).
 * This module relies on tick 0 slides and works in every version of
 * OctaMED between OctaMED 3.00 and MED Soundstudio 2.
 *
 * Each individual retrigger adds the current tick number to the hold counter.
 * FF2 note delay adds 3 to the hold counter or delays correctly (unsure).
 * Left and right should play the same for the entire module.
 *
 * Pattern 0: FF1
 * Pattern 1: FF3 (speed 3 only to avoid relying on FF3 bug)
 * Pattern 2: FF2
 */

TEST(test_player_med_hold_ff1_om300)
{
	compare_mixer_data(
		"data/med_hold_ff1_om300.med",
		"data/med_hold_ff1_om300.data");
}
END_TEST
