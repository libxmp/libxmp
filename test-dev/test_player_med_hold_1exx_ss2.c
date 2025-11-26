#include "test.h"

/* Test MED hold/decay plus pattern delay (1Exx).
 * This module relies on tick 0 slides and works in MED Soundstudio 2.
 *
 * Left and right should play the same for the entire module.
 *
 * Row 00-07: decay occurs normally on the pattern delay ticks.
 * Row 08-19: hold counts down normally on the pattern delay ticks.
 * Row 20-25: pattern delay on initial note does not affect hold count.
 * Row 26-39: pattern delay on sustained rows (rows preceding a hold symbol)
 *            sustains through the pattern delay ticks and into the next row.
 */

TEST(test_player_med_hold_1exx_ss2)
{
	compare_mixer_data(
		"data/med_hold_1exx_ss2.med",
		"data/med_hold_1exx_ss2.data");
}
END_TEST
