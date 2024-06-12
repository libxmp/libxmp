#include "test.h"

TEST(test_mixer_monoout_mono_16bit_linear)
{
	compare_mixer_samples(
		"data/mixer_monoout_mono_16bit_linear.data", "data/test.xm",
		8000, XMP_FORMAT_MONO, XMP_INTERP_LINEAR, TEST_XM_SAMPLE_16BIT_MONO, 0);
}
END_TEST
