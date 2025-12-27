#include "test.h"

/* Imago Orpheus IMF doesn't support S3M-style fine effects
 * syntax and uses its own syntax instead. It also doesn't
 * have S3M-style retrigger on count==0.
 *
 * IFF/JFF -> strong portamento up/down.
 * Kxx/Lxx -> fine portamento with 1/16th precision.
 * Axy     -> if x!=0 and y!=0, Orpheus uses (X - Y).
 * Rx0     -> Orpheus doesn't retrigger or change volume.
 *
 * TODO: bug, libxmp does apply the volume change on retrigger tick 0.
 */

TEST(test_effect_finefx_imf)
{
	compare_mixer_data(
		"data/finefx_imf.imf",
		"data/finefx_imf.data");
}
END_TEST
