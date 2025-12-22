#include "test.h"

/* Tests K00 and keyoff note equivalence on lines without
 * volume column toneporta for instruments with no envelope
 * and instruments with envelope (either sustaining or not).
 *
 * Pattern 0: no envelope
 * Pattern 1: trivial envelope
 * Pattern 2: sustain envelope
 * 00-0F: no volume/ins# -> no envelope cuts, everything else doesn't.
 * 10-1F: volume column set volume overrides cut (because cut sets volume to 0).
 * 20-2F: default volume overrides cut (because cut sets volume to 0).
 *
 * x0-x1: keyoff note
 * x2-x3: keyoff note + toneporta -> same as keyoff note
 * x4-x5: K00 -> same as keyoff note
 * x6-x7: note + K00 -> same as keyoff note
 * x8-xA: note + toneporta + K00 -> same as note + toneporta (K00 cleared)
 * xB-xC: toneporta + K00 -> same as toneporta (K00 cleared)
 * xD   : reset toneporta target
 * xE-xF: cut channel
 */

TEST(test_player_ft2_k00_is_note_off)
{
	compare_mixer_data(
		"data/ft2_k00_is_note_off.xm",
		"data/ft2_k00_is_note_off.data");
}
END_TEST
