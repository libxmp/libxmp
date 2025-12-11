#include "test.h"

/* Instrument numbers without a note do not update the current
 * playing sample, but they ALSO don't update the current
 * envelopes for the channel.
 */

TEST(test_player_ft2_no_note_ins_envelope)
{
	compare_mixer_data(
		"data/ft2_no_note_ins_envelope.xm",
		"data/ft2_no_note_ins_envelope.data");
}
END_TEST
