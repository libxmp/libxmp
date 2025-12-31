#include "test.h"

/* When an invalid instrument is active, note + toneporta still
 * updates the toneporta target. In this case, the default sample
 * transpose of +0 is used when computing the target.
 */

TEST(test_player_ft2_invalid_porta_target)
{
	compare_mixer_data(
		"data/ft2_invalid_porta_target.xm",
		"data/ft2_invalid_porta_target.data");
}
END_TEST
