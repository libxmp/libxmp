#include "test.h"

/* Instrument fade is applied to a channel on lines with
 * note + instrument number + no toneporta/K00. Instrument
 * memory, despite updating the active sample, will NOT
 * update this value (and possibly others).
 */

TEST(test_player_ft2_instrument_fade_update)
{
	compare_mixer_data(
		"data/ft2_instrument_fade_update.xm",
		"data/ft2_instrument_fade_update.data");
}
END_TEST
