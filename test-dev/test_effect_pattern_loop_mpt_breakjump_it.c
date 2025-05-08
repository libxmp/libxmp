#include "test.h"

TEST(test_effect_pattern_loop_mpt_breakjump_it)
{
	compare_mixer_data(
		"data/pattern_loop_mpt_breakjump.it",
		"data/pattern_loop_mpt_breakjump_it.data");
}
END_TEST
