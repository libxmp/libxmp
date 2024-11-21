#include "test.h"

/*
   Description: This test verifies that a volume command set together with
   an instrument but no note will be kept once the note is triggered
   later.
*/

TEST(test_openmpt_mod_ptinstrvolume)
{
	compare_mixer_data(
		"openmpt/mod/PTInstrVolume.mod",
		"openmpt/mod/PTInstrVolume.data");
}
END_TEST
