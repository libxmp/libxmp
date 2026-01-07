#include "test.h"

/* Ultra Tracker only "activates" one tone portamento effect
 * per event. The rate from the high FX takes priority over
 * the rate from the low FX.
 */

TEST(test_player_ult_double_toneporta)
{
	compare_mixer_data(
		"data/ult_double_toneporta.ult",
		"data/ult_double_toneporta.data");
}
END_TEST
