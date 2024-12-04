#include "test.h"

/*
   Description: When specifying an instrument number without a note,
   ProTracker 1/2 instantly applies the new instrument settings, but does
   not use the new sample until the end of the (loop of the) current
   sample is reached. In this example, sample 2 should be played at
   maximum volume as soon as instrument number 1 is encountered in the
   pattern, then sample 1 should be triggered somewhere around row 10 and
   then stop playing at around row 18.
*/

TEST(test_openmpt_mod_ptinstrswap)
{
	compare_mixer_data(
		"openmpt/mod/PTInstrSwap.mod",
		"openmpt/mod/PTInstrSwap.data");
}
END_TEST
