#include "test.h"

TEST(test_mixer_monoout_stereo_16bit_spline_filter)
{
	compare_mixer_samples(
		"data/mixer_monoout_stereo_16bit_spline_filter.data", "data/test.it",
		22050, XMP_FORMAT_MONO, XMP_INTERP_SPLINE, TEST_XM_SAMPLE_16BIT_STEREO, 1);
}
END_TEST
