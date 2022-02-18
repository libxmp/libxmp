#include "test.h"

/* A bidirectional sustain loop currently playing in reverse,
 * with the position past the end of the bidirectional main loop,
 * should not modify the current position in the sample when
 * sustain is released. Instead, the sample should continue playing
 * in reverse until it reaches the loop.
 * Also see OpenMPT SusAfterLoop.it.
 */

TEST(test_player_it_sus_after_loop_bidi)
{
	compare_mixer_data(
		"data/it_sus_after_loop_bidi.it",
		"data/it_sus_after_loop_bidi.data");
}
END_TEST
