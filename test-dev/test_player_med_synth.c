#include "test.h"

TEST(test_player_med_synth)
{
	compare_med_synth_data(
		"data/Inertiaload-1.med",
		"data/med_synth.data");
}
END_TEST
