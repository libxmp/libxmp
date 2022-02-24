#include "test.h"

/* Test that the OpenMPT IT extended effect \xx (Smooth MIDI Macro)
 * works correctly. Also uses custom parametered macros and Zxx.
 */

TEST(test_effect_it_smooth_macro)
{
	compare_mixer_data(
		"data/it_smooth_macro.it",
		"data/it_smooth_macro.data");
}
END_TEST
