#include "test.h"

TEST(test_player_med_synth_2)
{
	compare_med_synth_data(
		"data/MED.Synth-a-sysmic",
		"data/med_synth_2.data");
}
END_TEST
