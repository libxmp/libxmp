#include "test.h"

/* 1) High offset should NOT be applied unless Oxx is present.
 * 2) Offsets past the end of a sample set the offset to the end in old FX mode.
 */

TEST(test_player_it_high_offset_memory_oldfx)
{
	compare_mixer_data(
		"data/it_high_offset_memory_oldfx.it",
		"data/it_high_offset_memory_oldfx.data");
}
END_TEST
