#include "test.h"

TEST(test_player_ft2_note_off_after_invalid_ins)
{
	compare_mixer_data(
		"data/ft2_note_off_after_invalid_ins.xm",
		"data/ft2_note_off_after_invalid_ins.data");
}
END_TEST
