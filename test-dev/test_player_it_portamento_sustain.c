#include "test.h"

TEST(test_player_it_portamento_sustain)
{
	compare_mixer_data(
		"data/portamento_sustain.it",
		"data/portamento_sustain.data");
}
END_TEST
