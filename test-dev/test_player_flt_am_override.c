#include "test.h"

/* Left = Right
 *
 * 1) When an instrument has a sample and an AM instrument both specified,
 *    the AM instrument takes precedence (instrument 2; 00-05, left).
 * 2) When an AM instrument is truncated in the .NT file, assume 0 for the
 *    truncated fields P.FALL, V.AMP, V.SPD, FQ (instrument 4; 00-05, right).
 * 3) AM instruments overriding samples should not affect how the sample
 *    data is loaded from the module file (instrument 3; 06-07, right).
 *    In other words, when instrument 2 is loaded, its sample should either
 *    be fully loaded or fully skipped, and instrument 3 should sound
 *    identical to instrument 1.
 */

TEST(test_player_flt_am_override)
{
	compare_mixer_data(
		"data/flt_am_override.mod",
		"data/flt_am_override.data");
}
END_TEST
