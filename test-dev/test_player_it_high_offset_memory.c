#include "test.h"

/* 1) High offset should NOT be applied unless Oxx is present.
 * 2) Offsets past the end of a sample set the offset to 0 in new FX mode.
 */

TEST(test_player_it_high_offset_memory)
{
	compare_mixer_data(
		"data/it_high_offset_memory.it",
		"data/it_high_offset_memory.data");
}
END_TEST
