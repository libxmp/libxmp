#include "test.h"

TEST(test_effect_pattern_loop_liq_breakjump_no)
{
	compare_mixer_data(
		"data/pattern_loop_liq_breakjump_no.liq",
		"data/pattern_loop_liq_breakjump_no.data");
}
END_TEST
