#include "test.h"

/*
 This seems to be related to EnvOff.xm. Sound output should never go
 completely silent between the notes.

 Lachesis note: this claim seems to be dependent on tick-length ramping.
 Speed is 3, tick 0 of release is sustain, tick 1 is midway between
 points, tick 2 hits volume 0. libxmp output matches the *end* of the
 comparison sample ticks.
*/

TEST(test_openmpt_xm_pickup)
{
	compare_mixer_data(
		"openmpt/xm/Pickup.xm",
		"openmpt/xm/Pickup.data");
}
END_TEST
