#include "test.h"

/* This module is a mirror of the read_event_ft2 tests
 * (test_new_note*ft2.c, test_no_note*ft2.c, test_porta*ft2.c)
 * It doesn't need to be updated if those tests are updated,
 * but it is verified to play the same in libxmp and FT2.
 */

TEST(test_player_ft2_read_event)
{
	compare_mixer_data(
		"data/read_event_ft2.xm",
		"data/read_event_ft2.data");
}
END_TEST
