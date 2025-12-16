#include "test.h"

/* Volume column volume and pan mostly work as usual with Fasttracker 2
 * note delay. The one exception seems to be keyoff + no ins# + pan + delay,
 * which causes the pan effect to be ignored for some reason.
 *
 * Pattern 0 tests no envelope, pattern 1 tests envelope with no sustain,
 * and pattern 2 tests envelope with sustain. They all behave the same.
 *
 * 00-03: note/no note + no ins# + volume + delay -> works
 * 04-07: note/no note + no ins# + pan    + delay -> works
 * 08-0B: keyoff       + no ins# + volume + delay -> works
 * 0C-0F: keyoff       + no ins# + pan    + delay -> DOESN'T WORK
 * 10-13: note/no note + ins#    + volume + delay -> works
 * 14-17: note/no note + ins#    + pan    + delay -> works
 * 18-1B: keyoff       + ins#    + volume + delay -> works
 * 1C-1F: keyoff       + ins#    + pan    + delay -> works
 */

TEST(test_player_ft2_delay_volume_column)
{
	compare_mixer_data(
		"data/ft2_delay_volume_column.xm",
		"data/ft2_delay_volume_column.data");
}
END_TEST
