#include "test.h"

/* An active AM instrument envelope overrides whatever the
 * channel volume currently is, making these instruments
 * completely ignore default volume, Cxx/Axy effects, etc.
 *
 * AM instrument envelopes do not actually change the channel
 * volume, however: in StarTrekker, when the channel volume
 * changes, it briefly takes effect before the AM instrument
 * envelope overrides it (about 1/20th of a tick later).
 * This is difficult to support and almost unnoticable, so don't.
 */

TEST(test_player_flt_am_ignore_volfx)
{
	compare_mixer_data(
		"data/flt_am_ignore_volfx.mod",
		"data/flt_am_ignore_volfx.data");
}
END_TEST
