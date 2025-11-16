#include "test.h"

/* cxx and dxx should not have effects memory.
 * c00 and d00 do nothing.
 */
TEST(test_player_xm_no_cxx_dxx_memory)
{
	compare_mixer_data(
		"data/xm_no_cxx_dxx_memory.xm",
		"data/xm_no_cxx_dxx_memory.data");
}
END_TEST
