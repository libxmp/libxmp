#include "test.h"

/* FT2 only applies the offset effect for events with note + 9xx + !toneporta.
 * Unusually, however, it also only sets its MEMORY when offset is applied.
 * When the offset is past the end of the sample, the sample also cuts.
 *
 * 00-0C: various cases of note + !toneporta + 9xx, including cuts.
 * 0D   : do not apply offset (no note).
 * 0E-15: do not set memory with toneporta present.
 * 16-1F: more cases where toneporta memory should not be updated.
 *        Notably: DO update it when playing a note with an invalid instrument.
 *        (Memory is FF after this point.)
 * 22-24: >A#9, as usual, acts like there's no note i.e. for the purposes of
 *        offset, it does not set offset or update memory.
 *        (Memory is still FF after this point.)
 * 25-28: B-(-1), as usual, acts like whatever the last note was
 *        i.e. for the purposes of offset, it sets offset and updates memory.
 *        (Memory is overwritten with 01 here.)
 */

TEST(test_player_ft2_offset_memory)
{
	compare_mixer_data(
		"data/ft2_offset_memory.xm",
		"data/ft2_offset_memory.data");
}
END_TEST
