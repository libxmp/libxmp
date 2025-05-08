#include "test.h"

TEST(test_effect_pattern_loop_mpt_breakjump_xm)
{
	compare_mixer_data(
		"data/pattern_loop_mpt_breakjump.xm",
		"data/pattern_loop_mpt_breakjump_xm.data");
}
END_TEST
