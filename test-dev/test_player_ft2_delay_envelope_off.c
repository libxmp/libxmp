#include "test.h"

/* Test general delay behavior, particularly with regard to keyoff,
 * for instruments without envelopes. Pattern 0 tests delayed keyoff
 * with and without an instrument number; pattern 1 tests fadeout reset.
 *
 * 00-07: No instrument number: ED0 is ignored, ED1/ED2 with keyoff
 *        behave comparably to K01/K02 in this case.
 * 0A-11: Instrument number: ED0 is ignored, ED1/ED2 with keyoff
 *        behave comparably to not using keyoff at all in this case.
 *
 * 00-03: Delay with no note/no ins# is equivalent to delay with note.
 *        Both reset fadeout and retrigger the sample.
 * 04-07: Delay with keyoff/no ins#/no volume column resets fadeout + cuts
 *        volume to 0. EC1 is used for comparison but this doesn't test
 *        the fadeout reset portion very well.
 * 08-0B: Delay with no note and an ins# is equivalent to delay with a
 *        note and an ins#. Both reset fadeout and retrigger the sample.
 * 0C-0F: Delay with keyoff and an ins# also resets fadeout, but without
 *        retriggering the sample. There's no alternate representation
 *        for what this does, so compare with Fasttracker 2 or ft2-clone.
 *        (Minor: ft2-clone doesn't ramp the volume envelope here.)
 */

TEST(test_player_ft2_delay_envelope_off)
{
	compare_mixer_data(
		"data/ft2_delay_envelope_off.xm",
		"data/ft2_delay_envelope_off.data");
}
END_TEST
