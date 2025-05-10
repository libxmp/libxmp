#include "test.h"

TEST(test_effect_pattern_loop_imf_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_imf_breakjump.imf",
		"data/pattern_loop_imf_breakjump_imf.data");
}
END_TEST
