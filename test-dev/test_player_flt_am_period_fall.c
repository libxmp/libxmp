#include "test.h"

/* Player test for StarTrekker AM period fall handling.
 * StarTrekker simply adds the instrument's P.FALL
 * value to the current Amiga period once per tick.
 * (Note the displayed value is inverted; displayed "-8"
 * will increase the period and decrease the pitch by 8.)
 * This causes P.FALL to interact with portamento
 * commands, particularly toneporta.
 *
 * This also tests a strange bug of FQ, which is that
 * it affects new note periods but NOT toneporta target
 * periods. For FQ of 1 or greater, this can cause toneporta
 * to a lower note to actually INCREASE pitch.
 *
 * It isn't possible to simulate P.FALL with effects,
 * so compare against real StarTrekker playback.
 *
 * 00-07 : P.FALL decreasing pitch combined with C-3 320 (left)
 *         and 120 (right). Left = Right
 * 08-15 : P.FALL increasing pitch combined with C-1 310 (left)
 *         and 210 (right). Left = Right
 * 16-23 : P.FALL decreasing pitch combined with C-3 340 340 325 (left)
 *         and 140 140 125 (right). This should almost, but not quite,
 *         reach the target. If the FQ bug isn't simulated, the channels
 *         will desynchronize. FQ = 1, Left = Right
 * 24-31 : P.FALL increasing pitch combined with C-3 240 (left) and
 *         140 (right), then C-2 330 in both channels. Due to FQ = 1,
 *         the real target for the second toneporta will be C-3, but due
 *         to P.FALL, the playing note will be high enough that the
 *         second toneporta DECREASES pitch slightly. Left = Right
 * 32-39 : P.FALL decreasing pitch combined with C-1 380 (left) and
 *         180 (right). Due to FQ = 2, the target will be C-3, and the
 *         toneporta will actually INCREASE pitch. Left = Right
 */

TEST(test_player_flt_am_period_fall)
{
	compare_mixer_data(
		"data/flt_am_period_fall.mod",
		"data/flt_am_period_fall.data");
}
END_TEST
