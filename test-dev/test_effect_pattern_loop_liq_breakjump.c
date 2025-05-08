#include "test.h"

TEST(test_effect_pattern_loop_liq_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_liq_breakjump.liq",
		"data/pattern_loop_liq_breakjump.data");
}
END_TEST
