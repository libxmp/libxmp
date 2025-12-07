/* Common functions for the read event test_new_note_*,
 * test_no_note_*, and test_porta_* regression tests. */

#ifndef XMP_READ_EVENT_COMMON_H
#define XMP_READ_EVENT_COMMON_H

#include "../src/effects.h"
#include "../src/player.h"
#include "../src/mixer.h"
#include "../src/virtual.h"
#include "test.h"

#define KEY_C4		49
#define KEY_D4		51
#define KEY_C5		61
#define KEY_B5		72

#define INS_0		1
#define INS_1		2
#define INS_INVAL	3

#define INS_0_SUB_0_VOL	22
#define INS_0_SUB_1_VOL	11
#define INS_1_SUB_0_VOL	33

#define INS_0_SUB_0_PAN 0x40
#define INS_0_SUB_1_PAN 0xc0
#define INS_1_SUB_0_PAN 0xfe

#define INS_0_FADE	0x400
#define INS_1_FADE	0x1ffe

#define SET_VOL		43
#define SET_PAN		0x80

void create_read_event_test_module(struct context_data *ctx, int speed);

/* Note on and provided >=0 values match (called by check_new and check_on). */
void check_active(struct channel_data *xc, struct mixer_voice *vi,
		  int note, int ins, int vol, int pan, int ins_fade,
		  const char *desc);

/* check_active, but also checks that the note is NEW. */
void check_new(struct channel_data *xc, struct mixer_voice *vi,
	       int note, int ins, int vol, int pan, int ins_fade,
	       const char *desc);

/* check_active, but also checks that the note is NOT NEW. */
void check_on(struct channel_data *xc, struct mixer_voice *vi,
	      int note, int ins, int vol, int pan, int ins_fade,
	      const char *desc);

/* Note off; mixer channel is unmapped and set to volume 0. */
void check_off(struct channel_data *xc, struct mixer_voice *vi,
	       const char *desc);

#endif /* XMP_READ_EVENT_COMMON_H */
