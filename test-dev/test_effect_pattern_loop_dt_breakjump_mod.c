#include "test.h"

/* Since Digital Tracker has strange position jump bugs in
 * 6/8 channel mode, this test uses an "FA04" module.
 * It's not clear if Digital Tracker was ever capable of
 * creating these, but it loads them, and they can be
 * fingerprinted. */

TEST(test_effect_pattern_loop_dt_breakjump_mod)
{
	compare_mixer_data(
		"data/pattern_loop_dt_breakjump.mod",
		"data/pattern_loop_dt_breakjump_mod.data");
}
END_TEST
