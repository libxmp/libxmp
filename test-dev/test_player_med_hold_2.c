#include "test.h"

/* Test more MED hold/decay cases.
 * This module relies on tick 0 slides and works in every version of
 * MED/OctaMED between MED 3.00 and MED SoundStudio 2.
 *
 * Position 0 (pattern 1): left is hold/decay, right is volslide/cut.
 *
 * Hold 1, decay 6:   decay when hold ends (after 1 tick)
 * Hold 1, decay 0:   0 volume when hold ends (after 1 tick)
 * Hold 0, decay 12:  do not decay.
 * Hold 18, decay 12: decay when hold ends (after 18 ticks)
 *
 * Position 1 (pattern 0): left is hold/decay, right is volslide/cut.
 * Hold events work as follows: if the NEXT ROW has a hold event,
 * hold countdown is suspended for one row. The CURRENT ROW is
 * irrelevant (aside from setting the default volume in OctaMED 3.00
 * onward, not tested by this module).
 *
 * Hold 1, decay 6:   decay when hold ends (after 1 row + 1 tick;
 *                    slight left/right difference)
 * Hold 1, decay 0:   0 volume when hold ends (after 1 row + 1 tick)
 * Hold 0, decay 12:  do not decay
 * Hold 0, decay 0:   do not decay
 * Hold 18, decay 12: decay when hold ends (after 1 row + 18 ticks)
 * Hold 18, decay 12: decay when hold ends (after 3 rows + 18 ticks)
 * Hold 1, decay 6
 * with volslide A50: volslide affects decay! Decay just drops
 *                    the channel volume by the decay value.
 *
 * Position 2 (pattern 2): left is instrument hold/decay, right
 * is effect hold/decay. Effect hold/decay fully overrides the
 * instrument's hold/decay in all 16 cases, including if the
 * instrument has values of 0.
 *
 * Position 3 (pattern 3): left has no effect 8, right has effect 8
 * on rows without a note; otherwise the same. Effect 8 does nothing
 * in this case.
 *
 * Position 4 (pattern 4): left has hold/decay, right is volslide.
 * Decay stops once it reaches 0 volume; increasing the volume back
 * to 64 should not decay. Channels sound slightly different due
 * to phase differences, but the volume should be the same.
 */

TEST(test_player_med_hold_2)
{
	compare_mixer_data(
		"data/med_hold_2.med",
		"data/med_hold_2.data");
}
END_TEST
