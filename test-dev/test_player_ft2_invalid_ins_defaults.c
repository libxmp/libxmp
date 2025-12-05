#include "test.h"

/* When an invalid instrument is active, it sets default volume
 * and panning and probably other things, just like a valid
 * instrument. It always sets the volume=0 and panning=0x80.
 *
 * Referenced subinstruments seem to usually exist within an
 * instrument, at least when saved by FT2 and clone. They will
 * use whatever volume/panning they were saved with despite
 * also cutting the current channel (same as invalid instruments).
 *
 * This test exists to make it as obvious as possible that zeroing
 * volume for invalid instruments is not connected to the cutting
 * behavior; it is just normal default volume/pan.
 */

TEST(test_player_ft2_invalid_ins_defaults)
{
	compare_mixer_data(
		"data/ft2_invalid_ins_defaults.xm",
		"data/ft2_invalid_ins_defaults.data");
}
END_TEST
