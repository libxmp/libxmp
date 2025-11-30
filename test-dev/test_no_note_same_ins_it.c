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

TEST(test_no_note_same_ins_it)
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
	set_quirk(ctx, QUIRKS_IT, READ_EVENT_IT);

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

	/* Row 2: valid instrument with no note (IT)
	 *
	 * When a new valid instrument is the same as the current instrument
	 * and no note is set, IT keeps playing the current sample but
	 * sets the volume to the instrument's default volume.
	 *
	 * FIXME: check if envelope is being reset
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 INS_0_SUB_0_VOL, -1 /* FIXME: INS_0_SUB_0_PAN */, INS_0_FADE, "row 2");

	xmp_play_frame(opaque);

	/* Row 3: same, except subinstrument 1 (IT) */
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
		 INS_0_SUB_1_VOL, -1 /* FIXME: INS_0_SUB_1_PAN */, INS_0_FADE, "row 5");

	xmp_play_frame(opaque);


	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
