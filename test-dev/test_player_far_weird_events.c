#include "test.h"

/* Three weird edge cases that should be impossible to
 * insert into a Farandole Composer module from the editor.
 *
 * 1) instrument non-zero but no note: should do absolutely nothing.
 * 2) note with no volume: the volume is interpreted as 0. An example
 *    of this exists in order/pattern 16 of Prescience/aurora.far.
 * 3) note with volume >15: the volume is interpreted as 0 (usually?).
 *    This also causes graphical bugs in the fake VU meter.
 */

TEST(test_player_far_weird_events)
{
	compare_mixer_data(
		"data/far_weird_events.far",
		"data/far_weird_events.data");
}
END_TEST
