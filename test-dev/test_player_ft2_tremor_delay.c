#include "test.h"

/* More FT2 tremor+delay interaction tests.
 *
 * 00-0B: more verification that delay indeed resets the tremor counter.
 * 0C-17: ED0 is, as usual, a no-op. The tremor counter doesn't reset
 *        on these lines.
 * 18-23: EDx, x>=speed never resets the tremor counter because its row is
 *        never processed. However, it reveals something interesting:
 *        tremor does not continue updating in the ticks before the row
 *        is processed.
 * 24-2B: an example of the same quirk using ED2 at speed 3. The tremor
 *        state resets on tick 2 and *would* update on tick 1 if tremor
 *        continued to update on rows before the delay.
 */

TEST(test_player_ft2_tremor_delay)
{
	compare_mixer_data(
		"data/ft2_tremor_delay.xm",
		"data/ft2_tremor_delay.data");
}
END_TEST
