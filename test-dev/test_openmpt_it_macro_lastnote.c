#include "test.h"

/*
   A test of the MIDI macro letter “n”. This letter will
   always send the MIDI note value of the last triggered note, note cuts
   and similar “notes” are not considered. This module should remain
   silent as both channels should receive exactly the same cutoff values.
*/

TEST(test_openmpt_it_macro_lastnote)
{
	compare_mixer_data(
		"openmpt/it/macro-lastnote.it",
		"openmpt/it/macro-lastnote.data");
}
END_TEST
