#include "test.h"

TEST(test_effect_pattern_loop_st321_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_st321_breakjump.s3m",
		"data/pattern_loop_st321_breakjump.data");
}
END_TEST
