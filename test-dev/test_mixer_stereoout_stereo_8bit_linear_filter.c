#include "test.h"

TEST(test_mixer_stereoout_stereo_8bit_linear_filter)
{
	compare_mixer_samples(
		"data/mixer_stereoout_stereo_8bit_linear_filter.data", "data/test.it",
		22050, 0,  XMP_INTERP_LINEAR, TEST_XM_SAMPLE_8BIT_STEREO, 1);
}
END_TEST
