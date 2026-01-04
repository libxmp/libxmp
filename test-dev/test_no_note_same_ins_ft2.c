#include "read_event_common.h"

/*
Case 2: New instrument (no note)

  Instrument -> None    Same    Valid   Inval
PT1.1           -       Play    Play    Cut
PT1.3           -       NewVol  NewVol* Cut
PT2.3           -       NewVol  NewVol* Cut
PT3.15          -       NewVol  NewVol  Cut     <= "Standard"
PT3.61          -       NewVol  NewVol  Cut     <=
PT4b2           -       NewVol  NewVol  Cut     <=
MED             -       Hold    Hold    Cut%
FT2             -       OldVol  OldVol  OldVol
ST3             -       NewVol  NewVol  Cont
IT(s)           -       NewVol  NewVol  Cont
IT(i)           -       NewVol# Play    Cont
DT32            -       NewVol# NewVol# Cut

Play    = Play new note with new default volume
Switch  = Play new note with current volume
NewVol  = Don't play sample, set new default volume
OldVol  = Don't play sample, set old default volume
Cont    = Continue playing sample
Cut     = Stop playing sample

  * Protracker 1.3/2.3 queues sample changes immediately, but they don't take
    effect until the current playing sample completes its loop. This is
    supported by libxmp, as it shouldn't significantly hurt PT3 compatibility.

  # Don't reset envelope.

*/

TEST(test_no_note_same_ins_ft2)
{
	xmp_context opaque;
	struct context_data *ctx;
	struct player_data *p;
	struct channel_data *xc;
	struct mixer_voice *vi;
	int voc;

	opaque = xmp_create_context();
	ctx = (struct context_data *)opaque;
	p = &ctx->p;

	create_read_event_test_module(ctx, 2);
	new_event(ctx, 0, 0, 0, KEY_C5,       INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 1, 0, 0,            0,     0, FX_VOLSET, SET_VOL,
							FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 2, 0, 0,            INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 3, 0, KEY_B5,       INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 4, 0, 0,            0,     0, FX_VOLSET, SET_VOL,
							FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 5, 0, 0,            INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 6, 0, KEY_C5,       INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 7, 0, 0,            0,     0, FX_VOLSET, SET_VOL,
							FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 8, 0, XMP_KEY_OFF,  INS_0, 0, 0x00, 0, 0, 0);
	set_quirk(ctx, QUIRKS_FT2, READ_EVENT_FT2);

	xmp_start_player(opaque, XMP_MIN_SRATE, 0);

	/* Row 0 */
	xmp_play_frame(opaque);

	xc = &p->xc_data[0];
	voc = map_channel(p, 0);
	fail_unless(voc >= 0, "virtual map");
	vi = &p->virt.voice_array[voc];

	check_new(xc, vi, KEY_C5, INS_0,
		  INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 0");

	xmp_play_frame(opaque);

	/* Row 1: set non-default volume and pan */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 1");

	xmp_play_frame(opaque);

	/* Row 2: same instrument with no note (FT2)
	 *
	 * When a new valid instrument is the same as the current instrument
	 * and no note is set, FT2 keeps playing the current sample but
	 * sets the volume to the instrument's default volume.
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 2");

	xmp_play_frame(opaque);

	/* Row 3: same, except subinstrument 1 (FT2) */
	xmp_play_frame(opaque);
	check_new(xc, vi, KEY_B5, INS_0,
		  INS_0_SUB_1_VOL, INS_0_SUB_1_PAN, INS_0_FADE, "row 3");

	xmp_play_frame(opaque);

	/* Row 4: set non-default volume and pan */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_B5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 4");

	xmp_play_frame(opaque);

	/* Row 5 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_B5, INS_0,
		 INS_0_SUB_1_VOL, INS_0_SUB_1_PAN, INS_0_FADE, "row 5");

	xmp_play_frame(opaque);

	/* Row 6 */
	xmp_play_frame(opaque);
	check_new(xc, vi, KEY_C5, INS_0,
		  INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 6");

	xmp_play_frame(opaque);

	/* Row 7: set non-default volume and pan */
	/* FIXME: move to test_keyoff_same_ins_ft2.c */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 7");

	xmp_play_frame(opaque);

	/* Row 8: same instrument with key off (FT2)
	 *
	 * This behaves the same as same instrument with no note, except
	 * it also acts like key off.
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 -1, INS_0_SUB_0_PAN, INS_0_FADE, "row 8");

	fail_unless(xc->fadeout == 65536 - INS_0_FADE, "didn't start fade out");
	fail_unless(vi->vol == 346, "didn't fade out");
	xmp_play_frame(opaque);
	fail_unless(vi->vol == 341, "didn't fade out");

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
