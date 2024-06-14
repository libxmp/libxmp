#include "test.h"

TEST(test_mixer_stereoout_stereo_8bit_spline)
{
	compare_mixer_samples(
		"data/mixer_stereoout_stereo_8bit_spline.data", "data/test.xm",
		8000, 0, XMP_INTERP_SPLINE, TEST_XM_SAMPLE_8BIT_STEREO, 0);
}
END_TEST
