#include "test.h"

/*
   Description: ProTracker instrument swapping (see PTInstrSwap.mod
   test case) should also work when the "source" sample is not looped.
   However, when the "target" sample is not looped, sample playback should
   stop as with PTSwapEmpty.mod. Conceptually this can be explained
   because in this case, the sample loop goes from 0 to 2 in "oneshot"
   mode, i.e. it will loop a (hopefully) silent part of the sample.
*/

TEST(test_openmpt_mod_ptswapnoloop)
{
	compare_mixer_data(
		"openmpt/mod/PTSwapNoLoop.mod",
		"openmpt/mod/PTSwapNoLoop.data");
}
END_TEST
