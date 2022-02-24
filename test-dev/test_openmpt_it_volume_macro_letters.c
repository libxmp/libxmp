#include "test.h"

/*
   This test demonstrates how Zxx macros are parsed in
   Impulse Tracker:
     * Macros are evaluated only on the first tick.
     * They appear to be parsed after note / instrument information has
       been read from the pattern, but before any other effects (excluding
       "Set Volume").
     * 'u' and 'v' macros seem to emit at least a value of 1, not 0 - v00
       corresponds to Z01.
*/

TEST(test_openmpt_it_volume_macro_letters)
{
	compare_mixer_data(
		"openmpt/it/Volume-Macro-Letters.it",
		"openmpt/it/Volume-Macro-Letters.data");
}
END_TEST
