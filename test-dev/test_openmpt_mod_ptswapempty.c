#include "test.h"

/*
   Description: ProTracker instrument swapping (see PTInstrSwap.mod
   test case) also works when swapping from an empty sample slot back to a
   normal slot: If the sample was already swapped, it is restarted
   immediately.
*/

TEST(test_openmpt_mod_ptswapempty)
{
	compare_mixer_data(
		"openmpt/mod/PTSwapEmpty.mod",
		"openmpt/mod/PTSwapEmpty.data");
}
END_TEST
