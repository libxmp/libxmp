#include "test.h"

TEST(test_effect_pattern_loop_mpt_it)
{
	compare_mixer_data(
		"data/pattern_loop_mpt.it",
		"data/pattern_loop_mpt_it.data");
}
END_TEST
