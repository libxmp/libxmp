#include "test.h"

TEST(test_effect_pattern_loop_mpt_xm)
{
	compare_mixer_data(
		"data/pattern_loop_mpt.xm",
		"data/pattern_loop_mpt_xm.data");
}
END_TEST
