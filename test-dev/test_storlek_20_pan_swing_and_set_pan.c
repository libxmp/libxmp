#include "test.h"

/*
 A couple of brief notes about instrument pan swing: All of the values are
 calculated with a range of 0-64. Values out of the 0-64 range are clipped.
 The swing simply defines the amount of variance from the current panning
 value.

 Given all of this, a pan swing value of 16 with a center-panned (32)
 instrument should produce values between 16 and 48; a swing of 32 with full
 right panning (64) will produce values between 0 -- technically -32 -- and 32.

 However, when a set panning effect is used along with a note, it should
 override the pan swing for that note.

 This test should play sets of notes with: Hard left panning Left-biased
 random panning Hard right panning Right-biased random panning Center panning
 with no swing Completely random values
*/

#include "../src/rng.h"

#define FIXED_SEED 0x00006a36U

#ifndef FIXED_SEED
#include <limits.h>

#define PAN_SWING_RANGE 64

static inline unsigned brute_force_seed(void)
{
	struct rng_state rng;
	unsigned state = 0;
	unsigned val;
	int ok;
	int i;

	do {
		if (!(state & 0xffffff)) fprintf(stderr, "checking 0x%8xU...\n", state);
		libxmp_set_random(&rng, state);
		ok = 0;
		/* Discard - forced left pan */
		for (i = 0; i < 4; i++) {
			libxmp_get_random(&rng, 0);
		}
		/* Left pan - want highest possible value. */
		for (i = 0; i < 4; i++) {
			val = libxmp_get_random(&rng, PAN_SWING_RANGE + 1);
			if (val == PAN_SWING_RANGE)
				ok |= 1;
			if (val >= PAN_SWING_RANGE / 2 + 8 && val < PAN_SWING_RANGE)
				ok |= 2;
			if (val <= PAN_SWING_RANGE / 2)
				ok |= 4;
		}
		if (ok < 7) continue;

		ok = 0;
		/* Discard - forced right pan */
		for (i = 0; i < 4; i++) {
			libxmp_get_random(&rng, 0);
		}
		/* Right pan - want lowest possible value. */
		for (i = 0; i < 4; i++) {
			val = libxmp_get_random(&rng, PAN_SWING_RANGE + 1);
			if (val == 0)
				ok |= 1;
			if (val >= 1 && val <= PAN_SWING_RANGE / 2 - 8)
				ok |= 2;
			if (val >= PAN_SWING_RANGE / 2)
				ok |= 4;
		}
		if (ok < 7) continue;

		ok = 0;
		/* Discard - forced center pan */
		for (i = 0; i < 8; i++) {
			libxmp_get_random(&rng, 0);
		}
		/* Center pan - want highest and lowest possible value. */
		for (i = 0; i < 8; i++) {
			val = libxmp_get_random(&rng, PAN_SWING_RANGE + 1);
			if (val == 0)
				ok |= 1;
			if (val == PAN_SWING_RANGE)
				ok |= 2;
		}
		if (ok < 3) continue;

		libxmp_set_random(&rng, state);
		for (i = 0; i < 32; i++) {
			fprintf(stderr, "%2d: %d\n", i, libxmp_get_random(&rng, PAN_SWING_RANGE + 1));
		}

		return state;

	} while ((state++) < UINT_MAX);

	fprintf(stderr, "couldn't find seed!\n");
	exit(1);
}
#endif

TEST(test_storlek_20_pan_swing_and_set_pan)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct rng_state *rng;
	struct xmp_frame_info info;
	struct xmp_channel_info *ci;
	int values[64];
	int i;

	opaque = xmp_create_context();
	xmp_load_module(opaque, "data/storlek_20.it");
	xmp_start_player(opaque, 44100, 0);
	xmp_set_player(opaque, XMP_PLAYER_MIX, 100);

	/* This test has some broken statistical testing that
	 * isn't worth replacing right now--just preset a seed that
	 * produces extremes and a nice spread of values. -Lachesis */
	ctx = (struct context_data *)opaque;
	rng = &ctx->rng;
#ifdef FIXED_SEED
	libxmp_set_random(rng, FIXED_SEED);
#else
	libxmp_set_random(rng, brute_force_seed());
	fprintf(stderr, "\n#define FIXED_SEED 0x%08xU\n", rng->state);
#endif

	while (1) {
		xmp_play_frame(opaque);
		xmp_get_frame_info(opaque, &info);
		if (info.loop_count > 0)
			break;

		ci = &info.channel_info[0];
		if (info.frame == 0) {
			values[info.row] = ci->pan;
		}

	}

	/* Check if pan values are kept in the empty rows */
	for (i = 0; i < 64; i += 2) {
		fail_unless(values[i] == values[i + 1], "pan value not kept");
	}
	/* Check if left pan values are used */
	for (i = 0; i < 8; i++) {
		fail_unless(values[i] == 0, "pan left not set");
	}
	/* Check if left-biased pan values are used */
	for (i = 0; i < 8; i++) {
		fail_unless(values[8 + i] <= 128, "pan not left-biased");
	}
	/* Check if right pan values are used */
	for (i = 0; i < 8; i++) {
		fail_unless(values[16 + i] == 255, "pan right not set");
	}
	/* Check if right-biased pan values are used */
	for (i = 0; i < 8; i++) {
		fail_unless(values[24 + i] >= 127, "pan not right-biased");
	}
	/* Check if center pan values are used */
	for (i = 0; i < 16; i++) {
		fail_unless(values[32 + i] == 128, "pan center not set");
	}

	/* Check pan randomness */
	fail_unless(check_randomness(values +  8,  8, 10), "randomness error");
	fail_unless(check_randomness(values + 24,  8, 10), "randomness error");
	fail_unless(check_randomness(values + 48, 16, 10), "randomness error");

	xmp_end_player(opaque);
	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
