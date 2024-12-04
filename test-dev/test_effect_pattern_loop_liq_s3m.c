#include "test.h"

TEST(test_effect_pattern_loop_liq_s3m)
{
	compare_mixer_data(
		"data/pattern_loop_liq_s3m.liq",
		"data/pattern_loop_liq_s3m.data");
}
END_TEST
