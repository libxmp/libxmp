#include "test.h"

/*
   Description: Instrument swapping should happen instantly (not at the
   end of the sample or end of sample loop like in PTInstrSwap.mod)
   when there is an E9x retrigger command next to a lone instrument
   number. As with regular sample swapping, the sample finetune is not
   updated. The left and right channel of this module should sound
   identical.

   Note that the retrigger command can cause semi-random tiny delays with
   ProTracker on a real Amiga, so if there are small differences in phase
   between the left and right channel when playing this test in ProTracker
   but not in an external player, this is acceptable.
*/

TEST(test_openmpt_mod_instrswapretrigger)
{
	compare_mixer_data(
		"openmpt/mod/InstrSwapRetrigger.mod",
		"openmpt/mod/InstrSwapRetrigger.data");
}
END_TEST
