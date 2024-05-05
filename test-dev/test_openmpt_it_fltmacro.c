#include "test.h"

/*
   This test uses a custom macro configuration that uses the
   instrument volume to control the filter cutoff. A correctly implemented
   MIDI Macro system should pass this test.
*/

TEST(test_openmpt_it_fltmacro)
{
	compare_mixer_data(
		"openmpt/it/fltmacro.it",
		"openmpt/it/fltmacro.data");
}
END_TEST
