#include "test.h"

TEST(test_effect_pattern_loop_liq)
{
	compare_mixer_data(
		"data/pattern_loop_liq.liq",
		"data/pattern_loop_liq.data");
}
END_TEST
