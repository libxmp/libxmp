#include "test.h"

/*
Active invalid instruments don't reset the envelope positions:

0| C-4 01 --	play regular note
1| --- 7f --	invalid instrument sets default volume, resets envelope
2| C-4 -- --	activate invalid instrument; cut channel
3| --- 01 --
4| C-5 -- 40	Starts on tick (speed) of envelope; thus either row 1 reset
5|		the envelope positions and rows 2/3 did not update them [A];
6|		or row 3 also reset the envelope positions [B].
7|
8| C-4 7f --	activate invalid instrument; cut channel
9| --- 01 --
A| C-5 -- 40	envelope resumes from its position at the start of row 8,
		because neither of the previous rows reset the envelope
		position. Thus, [A] is correct.
*/

TEST(test_player_ft2_envelope_invalid_ins)
{
	compare_mixer_data(
		"data/ft2_envelope_invalid_ins.xm",
		"data/ft2_envelope_invalid_ins.data");
}
END_TEST
