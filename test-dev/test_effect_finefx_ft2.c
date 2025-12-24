#include "test.h"

/* FastTracker 2, OpenMPT, ModPlug Traker, MadTracker, MilkyTracker,
 * Skale Tracker, rst SoundTracker, Real Tracker 2, DigiTrakker, and
 * probably most other XM trackers do not support S3M-style fine effects
 * in XM modules. (Only FT2 tested here.)
 */

TEST(test_effect_finefx_ft2)
{
	compare_mixer_data(
		"data/finefx_ft2.xm",
		"data/finefx_ft2.data");
}
END_TEST
