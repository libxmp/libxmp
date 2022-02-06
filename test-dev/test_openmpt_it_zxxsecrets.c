#include "test.h"

/*
   Impulse Tracker supports three MIDI macro characters which
   are not documented in MIDI.TXT:
     * h: Host channel, i.e. the pattern channel in which the Zxx command
       is encountered (0-based).
     * o: The last used sample offset value. The high offset (SAx) is not
       taken into account. Note that offsets above 80h are not clamped, i.e.
       they generate MIDI command bytes (e.g. O90 would cause a note-on
       command to be emitted).
     * m: This command sends the current MIDI note if the channel is a MIDI
       channel, but on sample channels the current loop direction
       (forward = 0, backward = 1) of the sample is stored in the same memory
       location, so the macro evaluates to that instead of a note number.

   In addition, the MIDI messages FA (start song), FC (stop song) and
   FF (reset) reset the resonant filter parameters (cutoff = 127,
   resonance = 0) for all channels, but do not immediately update the filter
   coefficients.

   FIXME: libxmp gets the mixer test data correct but the audio test (libxmp's
   output and the IT WAV sample should cancel out) does not work, probably
   due to libxmp right shifting the cutoff by the filter envelope range
   instead of deriving coefficients off of the product.
*/

TEST(test_openmpt_it_zxxsecrets)
{
	compare_mixer_data(
		"openmpt/it/ZxxSecrets.it",
		"openmpt/it/ZxxSecrets.data");
}
END_TEST
