#include "test.h"

/*
   When a sample sustain loop, which is placed partly or
   completely behind a “normal” sample loop is exited (through a note-off
   event), and the current sample playback position is past the normal
   loop’s end, it is adjusted to current position - loop end + loop start.

   Non-advertized secondary breakage here: sample reverse needs to be
   canceled when exiting a bidirectional sustain loop into a regular loop
   that isn't bidirectional.
*/

TEST(test_openmpt_it_susafterloop)
{
	compare_mixer_data(
		"openmpt/it/SusAfterLoop.it",
		"openmpt/it/SusAfterLoop.data");
}
END_TEST
