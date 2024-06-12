#include "test.h"

TEST(test_mixer_stereoout_mono_16bit_spline_filter)
{
	compare_mixer_samples(
		"data/mixer_stereoout_mono_16bit_spline_filter.data", "data/test.it",
		22050, 0, XMP_INTERP_SPLINE, TEST_XM_SAMPLE_16BIT_MONO, 1);
}
END_TEST
