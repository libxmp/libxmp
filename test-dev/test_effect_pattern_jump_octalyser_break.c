#include "test.h"

/* Octalyser pattern jump resets break row to 0.
 */

TEST(test_effect_pattern_jump_octalyser_break)
{
	compare_mixer_data(
		"data/pattern_jump_octalyser_break.mod",
		"data/pattern_jump_octalyser_break.data");
}
END_TEST
