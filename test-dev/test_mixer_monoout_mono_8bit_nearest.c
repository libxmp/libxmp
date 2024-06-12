#include "test.h"

TEST(test_mixer_monoout_mono_8bit_nearest)
{
	compare_mixer_samples(
		"data/mixer_monoout_mono_8bit_nearest.data", "data/test.xm",
		8000, XMP_FORMAT_MONO, XMP_INTERP_NEAREST, TEST_XM_SAMPLE_8BIT_MONO, 0);
}
END_TEST
