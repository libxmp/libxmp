#include "test.h"

/*
 E1x, E2x, and 1xx memory should not be shared. Both channels should
 sound identical if effect memory is applied correctly.
*/

TEST(test_openmpt_xm_porta_linkmem_old)
{
	compare_mixer_data(
		"openmpt/xm/Porta-LinkMem_old.xm",
		"openmpt/xm/Porta-LinkMem_old.data");
}
END_TEST
