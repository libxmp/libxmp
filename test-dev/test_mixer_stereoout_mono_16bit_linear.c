#include "test.h"

TEST(test_mixer_stereoout_mono_16bit_linear)
{
	compare_mixer_samples(
		"data/mixer_stereoout_mono_16bit_linear.data", "data/test.xm",
		8000, 0, XMP_INTERP_LINEAR, TEST_XM_SAMPLE_16BIT_MONO, 0);
}
END_TEST
