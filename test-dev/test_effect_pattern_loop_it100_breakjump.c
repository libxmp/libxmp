#include "test.h"

TEST(test_effect_pattern_loop_it100_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_it100_breakjump.it",
		"data/pattern_loop_it100_breakjump.data");
}
END_TEST
