#include "test.h"

TEST(test_effect_pattern_loop_dt19)
{
	compare_mixer_data(
		"data/pattern_loop_dt19.dtm",
		"data/pattern_loop_dt19.data");
}
END_TEST
