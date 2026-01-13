#include "test.h"

/* Imperium Galactica toneporta is effectively identical to
 * Ultra Tracker toneporta, including double toneporta priority.
 * The rate from the high FX takes priority over the rate from the low FX.
 */

TEST(test_player_xmf_double_toneporta)
{
	compare_mixer_data(
		"data/xmf_double_toneporta.xmf",
		"data/xmf_double_toneporta.data");
}
END_TEST
