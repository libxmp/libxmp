#include "test.h"

TEST(test_effect_pattern_loop_dt_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_dt_breakjump.dtm",
		"data/pattern_loop_dt_breakjump_dtm.data");
}
END_TEST
