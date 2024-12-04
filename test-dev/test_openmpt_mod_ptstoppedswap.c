#include "test.h"

/*
   Description: When a non-looped sample stopped playing, it is still
   possible to perform sample swapping to a looped sample. As a result, on
   the first and third row, a square wave should be heard, a drum on the
   second and fourth row, but no new sample should be triggered on the
   fifth row.
*/

TEST(test_openmpt_mod_ptstoppedswap)
{
	compare_mixer_data(
		"openmpt/mod/PTStoppedSwap.mod",
		"openmpt/mod/PTStoppedSwap.data");
}
END_TEST
