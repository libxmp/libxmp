#include "test.h"

/* NOTE: libxmp plays this module wrong: the speed 1 version of the
 * jump before test doesn't work and plays once instead of four times.
 */

TEST(test_effect_pattern_loop_ptm_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_ptm_breakjump.ptm",
		"data/pattern_loop_ptm_breakjump.data");
}
END_TEST
