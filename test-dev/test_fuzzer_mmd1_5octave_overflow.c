#include "test.h"

/* This caused a signed left shift overflow in the MED octave instrument
 * loader due to using a signed int for an unsigned size field. This
 * happened with any sufficiently large octave instrument (not just 5 octave).
 */

TEST(test_fuzzer_mmd1_5octave_overflow)
{
	xmp_context opaque;
	int ret;

	opaque = xmp_create_context();
	ret = xmp_load_module(opaque, "data/f/load_mmd1_5octave_overflow.med");
	fail_unless(ret == 0, "module load");

	xmp_free_context(opaque);
}
END_TEST
