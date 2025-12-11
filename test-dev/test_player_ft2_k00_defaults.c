#include "test.h"

/* Lines with instrument number and K00 set default volume/pan.
 * Lines with instrument number and K00 DON'T reset the fadeout position.
 */

TEST(test_player_ft2_k00_defaults)
{
	compare_mixer_data(
		"data/ft2_k00_defaults.xm",
		"data/ft2_k00_defaults.data");
}
END_TEST
