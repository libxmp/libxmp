#include "test.h"

TEST(test_player_ft2_note_invalid_subins)
{
	compare_mixer_data(
		"data/ft2_note_invalid_subins.xm",
		"data/ft2_note_invalid_subins.data");
}
END_TEST
