#include "test.h"

TEST(test_effect_pattern_loop_ptm)
{
	compare_mixer_data(
		"data/pattern_loop_ptm.ptm",
		"data/pattern_loop_ptm.data");
}
END_TEST
