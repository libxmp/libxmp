#include "test.h"

/* Bxx+Cxx works in either order.
 */

TEST(test_effect_pattern_jump_st3_break)
{
	compare_mixer_data(
		"data/pattern_jump_st3_break.s3m",
		"data/pattern_jump_st3_break.data");
}
END_TEST
