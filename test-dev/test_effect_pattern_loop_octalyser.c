#include "test.h"

/* NOTE: this module is incomplete because libxmp
 * doesn't fully emulate some of Octalyser's strange
 * (and kind of useless) pattern loop behavior yet.
 */

TEST(test_effect_pattern_loop_octalyser)
{
	compare_mixer_data(
		"data/pattern_loop_octalyser.mod",
		"data/pattern_loop_octalyser.data");
}
END_TEST
