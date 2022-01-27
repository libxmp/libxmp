#include "test.h"

TEST(test_player_it_portamento_offset)
{
	compare_mixer_data(
		"data/portamento_offset.it",
		"data/portamento_offset.data");
}
END_TEST
