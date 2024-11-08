#include "test.h"

/* Test Ultra Tracker tempo effects.
 */

TEST(test_effect_ult_tempo)
{
	compare_mixer_data(
		"data/ult_effectF.ult",
		"data/ult_effectF.data");
}
END_TEST
