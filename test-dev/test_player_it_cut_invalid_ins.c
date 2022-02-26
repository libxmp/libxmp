#include "test.h"

/* Note cut, note off, and note fade should be performed regardless
 * of whether or not they are provided with an invalid instrument.
 */

TEST(test_player_it_cut_invalid_ins)
{
	compare_mixer_data(
		"data/it_cut_invalid_ins.it",
		"data/it_cut_invalid_ins.data");
}
END_TEST
