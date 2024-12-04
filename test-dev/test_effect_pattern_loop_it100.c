#include "test.h"

TEST(test_effect_pattern_loop_it100)
{
	compare_mixer_data(
		"data/pattern_loop_it100.it",
		"data/pattern_loop_it100.data");
}
END_TEST
