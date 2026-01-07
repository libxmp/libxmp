#include "test.h"

/* Combining Mx+3xx/5xy results in both effects advancing toneporta.
 * Fasttracker 2 has a bug, however, that the 3xx in this case will
 * use the rate set by Mx. In other words, Mx+3xx effectively applies
 * double whatever the Mx rate is (including from memory).
 */

TEST(test_player_ft2_double_toneporta)
{
	compare_mixer_data(
		"data/ft2_double_toneporta.xm",
		"data/ft2_double_toneporta.data");
}
END_TEST
