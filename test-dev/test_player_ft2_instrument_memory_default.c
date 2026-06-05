#include "test.h"

/* Instrument memory defaults to 0 (no instrument) for all
 * channels at playback start. Left = Right
 */

TEST(test_player_ft2_instrument_memory_default)
{
	compare_mixer_data(
		"data/ft2_instrument_memory_default.xm",
		"data/ft2_instrument_memory_default.data");
}
END_TEST
