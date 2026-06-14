#include "test.h"

/* Player test for StarTrekker AM envelope emulation.
 * Aside from popping in the original tracker, left = right.
 *
 * 00-16   left begins at 40, incs. by 2 to FF; right begins at
 *         20, incs. by 1 to 80 (doubled). They should stop on
 *         the exact same tick.
 * 20-23   Speed = 0, prev level != level should stop the envelope.
 * 24-25   Attack 1 lasts a minimum of 1 tick.
 * 26-27   Attack 1 + Attack 2 lasts a minimum of 2 ticks.
 * 28-29   Attack 1 + Attack 2 + Decay + Sustain lasts a minimum of 4 ticks.
 * 30-31   Same, but with a sustain time of 2 -> 6 ticks.
 * 32-40   Attack 1 is applied immediately. Also tests that a speed of 0
 *         should NOT stop the envelope if prev level == level.
 */

TEST(test_player_flt_am_envelope)
{
	compare_mixer_data(
		"data/flt_am_envelope.mod",
		"data/flt_am_envelope.data");
}
END_TEST
