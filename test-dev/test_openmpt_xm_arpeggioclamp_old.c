#include "test.h"

/*
 Arpeggio parameters are clamped differently than the base note. Not sure how
 this worked, it has been ages since I fixed this.
*/

TEST(test_openmpt_xm_arpeggioclamp_old)
{
	compare_mixer_data(
		"openmpt/xm/ArpeggioClamp_old.xm",
		"openmpt/xm/ArpeggioClamp_old.data");
}
END_TEST
