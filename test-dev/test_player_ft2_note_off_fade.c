#include "test.h"

/* Keyoff always fades immediately--previously, libxmp had this
 * occur indirectly across a flag chain that could delay fadeout
 * from starting for up to two ticks.
 *
 * Pattern 0: no envelope
 * 00-0B: keyoff starts fadeout. Volume blocks cutting the volume
 *        to 0, but not the fadeout.
 * 0C-17: keyoff starts fadeout. Instrument numbers block cutting
 *        the volume to 0 and reset fadeout, unless they are on
 *        the same line as the keyoff, in which case fadeout stays.
 * 18-23: keyoff is unaffected by toneporta.
 * 24-2F: same as 00-0B, except with K01 instead of keyoff. Volume
 *        should cancel the cut in this case, because it's also just
 *        setting volume to 0 on one tick!
 * 30-32: keyoff + delay blocks fadeout.
 * 33-37: keyoff + delay resets fadeout.
 *
 * Pattern 1 (trivial envelope) and pattern 2 (sustain):
 * 00-13: a shorter version of pattern 0 00-2F. The left channel
 *        uses a regular volume envelope or a sustained volume envelope,
 *        while the right mostly uses the same no-envelope sample as 0.
 *        Keyoff always starts fadeout at the same time for all cases.
 * 14-16: keyoff + delay blocks fadeout.
 * 17-1B: keyoff + delay resets fadeout.
 */

TEST(test_player_ft2_note_off_fade)
{
	compare_mixer_data(
		"data/ft2_note_off_fade.xm",
		"data/ft2_note_off_fade.data");
}
END_TEST
