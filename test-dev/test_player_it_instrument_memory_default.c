#include "test.h"

/* Instrument memory defaults to 0 (no instrument) for all
 * channels at playback start. Left = Right
 */

TEST(test_player_it_instrument_memory_default)
{
	compare_mixer_data(
		"data/it_instrument_memory_default.it",
		"data/it_instrument_memory_default.data");
}
END_TEST
