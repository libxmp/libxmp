#include "test.h"

/*
   Even fixed macros (Z80-ZFF) can contain the letter “z”,
   which inserts the raw command parameter into the macro (i.e. a value
   in [80, FF[). In this file, macro ZF0 is used to insert byte F0 into
   the string. This way, two MIDI messages to set both the filter cutoff
   and resonance to 60h are created, which are the same filter settings
   as used in instrument 2, so the module should stay silent.
*/

TEST(test_openmpt_it_macro_extended_param)
{
	compare_mixer_data(
		"openmpt/it/MacroExtendedParam.it",
		"openmpt/it/MacroExtendedParam.data");
}
END_TEST
