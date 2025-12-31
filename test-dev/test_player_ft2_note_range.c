#include "test.h"

/* FT2 tests the transposed note range *before* updating the current
 * key/instrument/sample and note, near the start of event handling.
 *
 * 1) If the transposed note is >=C-0 and <=A#9, the new
 *    key/instrument/sample and the new note are accepted.
 * 2) If the transposed note is B-(-1), the new key/instrument/sample are
 *    accepted, but the old transposed note value is retained. A new note
 *    is played with the new instrument/sample reusing the old note value.
 * 3) If the transposed note is <B-(-1) or >A#9, the new key is discarded.
 *    The rest of the line processes as if the event has no key, so the
 *    old note continues to play.
 *
 * Any of these cases can be reused in EDx note memory.
 *
 * Pattern 0: general note ranges
 * 00-0B: transposed B-9 is the first out of range note.
 * 0C-13: transposed B-9 acts like no key is present in the event.
 * 14-1F: transposed B-(-1) is the first out of range note.
 * 20-23: transposed B-(-1) acts like a new note, but retains the old
 *        calculated note value.
 * 24-2B: transposed B-(-1) changes the instrument, A#(-1) does not;
 *        A#(-1) acts like B-9. The new instrument/sample remain when
 *        a note is played without an instrument number.
 * 2C-2F: transposed C-(-4) acts like A#(-1), B-9, etc. B-(-1) is probably
 *        the only special out-of-range note.
 *
 * Pattern 1: note memory 1
 * 00-04: A key that transposes to B-9 is stored to delay note memory.
 *        None of these delays retrigger, instead acting more similar to
 *        keyoff retrigger.
 * 05-06: WEIRD EDGE CASE: delay notes that resolve to an out-of-range value
 *        seem to temporarily mute the playing channel until the next time the
 *        volume updates. This does *not* set the channel volume, as +0 is able
 *        to recover the set volume. TODO: libxmp does not support this.
 * 07   : the old note also persists for the purposes of arpeggio.
 * 0A-0E: A key that transposes to B-(-1) is stored to delay note memory.
 *        These delays retrigger, but the old transposed note value is retained.
 * 0F-10: B-(-1) delay note memory is unaffected by the weird muting bug.
 * 11   : the old note also persists for the purposes of arpeggio.
 *
 * Pattern 2: note memory 2
 * 00-03: A key that transposes to B-9 is stored in delay note memory.
 *        The delay on row 3 does not retrigger.
 * 04-07: Toneporta blocks updating note memory. Row 7 DOES retrigger.
 * 08-0B: Same, except volume column. Row A DOES retrigger.
 * 0C-0F: K00 blocks updating note memory (updated after K00 note overwrite).
 *        Row E DOES retrigger.
 */

TEST(test_player_ft2_note_range)
{
	compare_mixer_data(
		"data/ft2_note_range.xm",
		"data/ft2_note_range.data");
}
END_TEST
