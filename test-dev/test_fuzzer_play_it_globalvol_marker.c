#include "test.h"


TEST(test_fuzzer_play_it_globalvol_marker)
{
	/* This module is based on fuzzer files that found the following bugs:
	 * - When there are 256 orders in a module with markers, libxmp would
	 *   read beyond the order array to look for an end marker.
	 * - When a module repeats into a marker, libxmp would derive the new
	 *   global volume from invalid data on the marker.
	 */
	compare_mixer_data_loops(
		"data/f/play_it_globalvol_marker.it",
		"data/f/play_it_globalvol_marker.data",
		3);
}
END_TEST
