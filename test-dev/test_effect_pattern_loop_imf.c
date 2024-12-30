#include "test.h"

TEST(test_effect_pattern_loop_imf)
{
	compare_mixer_data(
		"data/pattern_loop_imf.imf",
		"data/pattern_loop_imf_imf.data");
}
END_TEST
