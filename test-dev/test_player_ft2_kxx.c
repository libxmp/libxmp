#include "test.h"

/* Tests some edge cases of FT2 Kxx.
 *
 * Pattern 0: no envelope
 * Pattern 1: trivial envelope
 * Pattern 2: sustain envelope
 *
 * 00-03: Kxx no-envelope cut is reset by volume like keyoff.
 *        This does not reset fadeout.
 * 04-07: Kxx==speed is ignored.
 * 08-0B: Kxx>speed is ignored.
 * 0C-0F: Kxx fadeout is reset by keyoff+delay.
 * 10-13: Kxx>=speed is ignored during the next row's delay ticks.
 * 14-17: Kxx>=speed is ignored on pattern delay row repeats.
 * 18-1B: Alternate test for Kxx>=speed + delay.
 */

TEST(test_player_ft2_kxx)
{
	compare_mixer_data(
		"data/ft2_kxx.xm",
		"data/ft2_kxx.data");
}
END_TEST
