#include "test.h"

TEST(test_effect_pattern_loop_mpt_breakjump_s3m)
{
	compare_mixer_data(
		"data/pattern_loop_mpt_breakjump.s3m",
		"data/pattern_loop_mpt_breakjump_s3m.data");
}
END_TEST
