#include "test.h"

/* Test general delay behavior, particularly with regard to keyoff,
 * for instruments with envelopes and active sustain. Pattern 0 tests
 * delayed keyoff with and without an instrument number; pattern 1
 * tests release reset; pattern 2 tests fadeout reset.
 *
 * 00-0F: No instrument number: ED0 is ignored, ED1/ED2 with keyoff
 *        does not release sustain.
 * 14-24: Instrument number: ED0 is ignored, ED1/ED2 with keyoff
 *        does not release sustain.
 * 28-2F: Delay with no note/instrument resets the envelope positions.
 *
 * 00-03: Delay with no note/no ins# is equivalent to delay with note.
 *        Both reset envelope positions and reset release and retrigger.
 * 04-07: Delay with keyoff/no ins# resets envelopes/release without
 *        retriggering the sample. There's no alternate representation
 *        for what this does, so compare with Fasttracker 2 or ft2-clone
 *        (Minor: ft2-clone ramps the volume envelope here.)
 * 08-0B: Delay with no note and an ins# is equivalent to delay with a
 *        note and an ins#. Both reset envelopes/release and retrigger.
 * 0C-0F: Delay with keyoff and an ins# resets envelopes/release without
 *        retriggering the sample. There's no alternate representation
 *        for what this does, so compare with Fasttracker 2 or ft2-clone.
 *        (Minor: ft2-clone doesn't ramp the volume envelope here.)
 *
 * 00-03: Delay with no note/no ins# is equivalent to delay with note.
 *        Both reset fadeout and retrigger the sample.
 * 04-07: Delay with keyoff/no ins# also resets fadeout, but without
 *        retriggering the sample. There's no alternate representation
 *        for what this does, so compare with Fasttracker 2 or ft2-clone
 *        (Minor: ft2-clone ramps the volume envelope here.)
 * 08-0B: Delay with no note and an ins# is equivalent to delay with a
 *        note and an ins#. Both reset fadeout and retrigger the sample.
 * 0C-0F: Delay with keyoff and an ins# also resets fadeout, but without
 *        retriggering the sample. There's no alternate representation
 *        for what this does, so compare with Fasttracker 2 or ft2-clone.
 *        (Minor: ft2-clone doesn't ramp the volume envelope here.)
 */

TEST(test_player_ft2_delay_envelope_sustain)
{
	compare_mixer_data(
		"data/ft2_delay_envelope_sustain.xm",
		"data/ft2_delay_envelope_sustain.data");
}
END_TEST
