#include "test.h"

TEST(test_effect_pattern_loop_liq_no)
{
	compare_mixer_data(
		"data/pattern_loop_liq_no.liq",
		"data/pattern_loop_liq_no.data");
}
END_TEST
