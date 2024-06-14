#include "test.h"

TEST(test_mixer_monoout_stereo_16bit_spline)
{
	compare_mixer_samples(
		"data/mixer_monoout_stereo_16bit_spline.data", "data/test.xm",
		8000, XMP_FORMAT_MONO, XMP_INTERP_SPLINE, TEST_XM_SAMPLE_16BIT_STEREO, 0);
}
END_TEST
