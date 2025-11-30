#include "test.h"

/* New notes (NOT instruments) cancel persistent volume slides.
 * They probably also cancel other persistent effects, but this
 * only tests volslide up and down.
 *
 * 00-03 : play note (vol 0xff) with volslide down
 * 04-05 : new note (vol 0xff) should cancel vslide down
 * 06-07 : play note (vol 0xff) with volslide down
 * 08-09 : new note (vol 0x3f) should cancel vslide down
 * 0a-0b : play note (vol 0x3f) with no volslide
 * 0c-0d : new note (vol 0xff) with volslide down
 * 0e-0f : instrument-only event (vol 0xff) sets vol, doesn't cancel effect
 * 10-17 : reload event (vol 0xff) sets vol, doesn't cancel effect
 *
 * TODO: libxmp has two known bugs here!
 * - The volume slide values are wrong, FunkTracker seems to use a table.
 * - libxmp cancels the effect at row 0Eh, it should continue until the end. */

TEST(test_player_fnk_note_vslide_cancel)
{
	compare_mixer_data(
		"data/fnk_note_vslide_cancel.fnk",
		"data/fnk_note_vslide_cancel.data");
}
END_TEST
