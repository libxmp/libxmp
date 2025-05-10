#include "test.h"

TEST(test_effect_pattern_loop_st301_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_st301_breakjump.s3m",
		"data/pattern_loop_st301_breakjump.data");
}
END_TEST
