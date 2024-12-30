#include "test.h"

TEST(test_effect_pattern_loop_dt)
{
	compare_mixer_data(
		"data/pattern_loop_dt.dtm",
		"data/pattern_loop_dt_dtm.data");
}
END_TEST
