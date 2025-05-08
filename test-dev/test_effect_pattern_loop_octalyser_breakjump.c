#include "test.h"


TEST(test_effect_pattern_loop_octalyser_breakjump)
{
	compare_mixer_data(
		"data/pattern_loop_octalyser_breakjump.mod",
		"data/pattern_loop_octalyser_breakjump.data");
}
END_TEST
