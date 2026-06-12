#include "test.h"

/* Left = Right
 *
 * A quick and dirty test for StarTrekker's new note/ins behavior
 * for AM instruments.
 *
 * 00-15 : New note (same ins, valid ins, invalid ins, no ins)
 * 16-27 : No note (same ins, valid ins, invalid ins)
 * 32-47 : Porta (no ins, same ins, valid ins, invalid ins)
 *
 * TODO: replace this with new_note* / no_note* / porta* tests once
 * the sampled behavior for StarTrekker and other early trackers can
 * be examined in more detail for a new read_event function.
 */

TEST(test_player_flt_am_new_note)
{
	compare_mixer_data(
		"data/flt_am_new_note.mod",
		"data/flt_am_new_note.data");
}
END_TEST
