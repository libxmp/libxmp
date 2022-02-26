#include "test.h"

/* Don't allow note delay to carry into NNA channels.
 */

TEST(test_player_it_note_delay_nna)
{
	compare_mixer_data(
		"data/it_note_delay_nna.it",
		"data/it_note_delay_nna.data");
}
END_TEST
