#include "test.h"

/* Test MED hold/decay plus retrigger (FF1/FF3) and note delay (FF2)
 * This module relies on tick 0 slides and works in all MED/OctaMED
 * versions from MED 3.00/OctaMED 1.00 until OctaMED 3.00.
 *
 * Retrigger does not increase the hold counter in these old versions.
 * Note delay seems to disable hold entirely for some reason.
 * Left and right should play the same for the entire module.
 *
 * Pattern 0: FF1
 * Pattern 1: FF3 (speed 3 only to avoid relying on FF3 bug)
 * Pattern 2: FF2 (TODO: not supported)
 */

TEST(test_player_med_hold_ff1_old)
{
	compare_mixer_data(
		"data/med_hold_ff1_old.med",
		"data/med_hold_ff1_old.data");
}
END_TEST
