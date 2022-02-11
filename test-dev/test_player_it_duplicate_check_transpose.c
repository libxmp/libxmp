#include "test.h"

/* Duplicate note check is based on the key used to play the
 * note, not the note played after transpose. Only the exact
 * same key should trigger the duplicate check action in this
 * module; keys transposed to the same note should not.
 */

TEST(test_player_it_duplicate_check_transpose)
{
	compare_mixer_data(
		"data/duplicate_check_transpose.it",
		"data/duplicate_check_transpose.data");
}
END_TEST
