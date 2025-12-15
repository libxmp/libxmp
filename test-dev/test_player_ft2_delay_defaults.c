#include "test.h"

/* Instrument defaults work the same as normal on delay rows.
 * Pattern 0 tests no envelope, pattern 1 tests envelope.
 *
 * 00-07: "Note memory"/"retrigger" rows without ins# do not apply defaults.
 * 08-0A: Keyoff without ins# rows do not apply defaults.
 *
 * 10-13: "Note memory"/"retrigger" rows with ins# apply defaults.
 * 14-17: Keyoff with ins# rows apply defaults.
 */

TEST(test_player_ft2_delay_defaults)
{
	compare_mixer_data(
		"data/ft2_delay_defaults.xm",
		"data/ft2_delay_defaults.data");
}
END_TEST
