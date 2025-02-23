#include "test.h"

/* MED and OctaMED have 4 retrigger and delay effects. The original
 * three (FF1, FF2, FF3) were added to the original MED when it could
 * play at speed 6 only, and they make more sense in that context.
 * 1Fxy was added much later to the MMD1 format.
 *
 *   FF1: Play Note Twice. Retriggers ONCE ONLY on tick 3.
 *        Works on lines without a note.
 *   FF2: Delay Note. Delays the note until tick 3.
 *   FF3: Play Note Three Times. Intended implementation is
 *        to retrigger on ticks 2 and 4. In OctaMED 5.00 and up, this
 *        retriggers every 2 ticks like 1F02, except it works on lines
 *        without a note. In all prior MED/OctaMED versions, it is
 *        buggy and retriggers on every tick where (tick & 7) >= 2.
 *  1Fxy: Note Delay and Retrigger. MED will delay x ticks and then
 *        play the initial note, retriggering every y ticks. This
 *        effect does nothing on lines that don't have a note.
 *
 * Currently, libxmp only implements the OctaMED 5.00 (sane) behavior.
 * Left and right output should be nearly identical; this relies on
 * FF1/FF2/FF3 playing correctly as well as both delay and retrigger
 * working on the same note when played with 1Fxy.
 */

TEST(test_effect_med_retrig)
{
	compare_mixer_data(
		"data/med_retrig_500.mmd1",
		"data/med_retrig_500.data");
}
END_TEST
