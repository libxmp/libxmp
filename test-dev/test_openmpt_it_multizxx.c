#include "test.h"

/*
   A MIDI macro can contain more than one MIDI message. In
   this case, the Z90 macro sets both the filter cutoff frequency and
   resonance, so if only the first MIDI message is considered in this
   macro, the module will no longer stay silent at row 8.
*/

TEST(test_openmpt_it_multizxx)
{
	compare_mixer_data(
		"openmpt/it/MultiZxx.it",
		"openmpt/it/MultiZxx.data");
}
END_TEST
