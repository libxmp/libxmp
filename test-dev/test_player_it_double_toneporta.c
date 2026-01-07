#include "test.h"

/* Combining Gx+Gxx/Lxy results in both effects advancing toneporta.
 * IT has the same bug as FT2 where the shared toneporta memory is
 * reused by both effects, but unlike FT2, it will correctly update
 * memory from both parameters on the start tick.
 * TODO: libxmp gets rows 00-0F wrong because it does not reload
 * the toneporta memory rate for each effect every tick.
 */

TEST(test_player_it_double_toneporta)
{
	compare_mixer_data(
		"data/it_double_toneporta.it",
		"data/it_double_toneporta.data");
}
END_TEST
