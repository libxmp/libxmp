#include "test.h"

TEST(test_player_it_default_volume)
{
	compare_mixer_data(
		"data/default_volume.it",
		"data/default_volume_it.data");
}
END_TEST
