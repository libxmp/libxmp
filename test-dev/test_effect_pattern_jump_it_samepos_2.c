#include "test.h"

/* The same as pattern_jump_it_samepos.it, but with the
 * order of Bxx/Cxx reversed. It should play identically.
 */

TEST(test_effect_pattern_jump_it_samepos_2)
{
	compare_mixer_data(
		"data/pattern_jump_it_samepos_2.it",
		"data/pattern_jump_it_samepos.data");
}
END_TEST
