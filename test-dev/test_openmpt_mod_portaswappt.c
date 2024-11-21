#include "test.h"

/*
   Description: Related to PortaSmpChange.mod, this tests the
   portamento with sample swap behaviour for ProTracker-compatible MODs.
   Noteworthy tested behaviours:
     * When doing a sample swap without portamento, the new sample keeps
       using the old sample’s finetune.
     * When doing a sample swap with portamento, the new sample’s finetune
       is instantly applied, and the new sample is started as soon as the
       old sample’s loop is finished playing.
*/

TEST(test_openmpt_mod_portaswappt)
{
	compare_mixer_data(
		"openmpt/mod/PortaSwapPT.mod",
		"openmpt/mod/PortaSwapPT.data");
}
END_TEST
