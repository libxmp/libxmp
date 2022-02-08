#include "test.h"

TEST(test_player_it_channel_filter)
{
	compare_mixer_data(
		"data/it_channel_filter.it",
		"data/it_channel_filter.data");
}
END_TEST
