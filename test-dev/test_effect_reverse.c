#include "test.h"

/* Test the Modplug extended effects S9E/X9E Play Forward and
 * S9F/X9E Play Reverse (particularly how S9F interacts with
 * IT loops). Also make sure similar reverse effects for other
 * tracker formats render roughly how they're supposed to.
 */

TEST(test_effect_reverse)
{
	compare_mixer_data(
		"data/reverse_it.it",
		"data/reverse_it.data");
	compare_mixer_data(
		"data/reverse_xm.xm",
		"data/reverse_xm.data");
	compare_mixer_data(
		"data/reverse_mmd3.med",
		"data/reverse_mmd3.data");
}
END_TEST
