#include "read_event_common.h"

/*
Case 1: New note

  Instrument -> None    Same    Valid   Inval
PT1.1           Play    Play    Play    Cut
PT1.3           Play    Play    Play    Cut
PT2.3           Switch  Play    Play    Cut     <=
PT3.15          Switch  Play    Play    Cut     <= "Standard"
PT3.61          Switch  Play    Play    Cut     <=
PT4b2           Switch  Play    Play    Cut     <=
MED             Switch  Play    Play    Cut     <=
FT2             Switch  Play    Play    Cut     <=
ST3             Switch  Play    Play    Switch
IT(s)           Switch  Play    Play    ?
IT(i)           Switch  Play    Play    Cont
DT32            Play    Play    Play    Cut

Play    = Play new note with new default volume
Switch  = Play new note with current volume
NewVol  = Don't play sample, set new default volume
OldVol  = Don't play sample, set old default volume
Cont    = Continue playing sample
Cut     = Stop playing sample

*/

TEST(test_new_note_invalid_ins_it)
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
	new_event(ctx, 0, 0, 0, KEY_C5, INS_0,     0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 1, 0, 0,      0,         0, FX_VOLSET, SET_VOL,
						      FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 2, 0, KEY_D4, INS_INVAL, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 3, 0, KEY_C4, 0,         0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 4, 0, KEY_C5, INS_0,     0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 5, 0, KEY_B5, INS_1,     0, 0x00, 0, 0, 0);
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

	/* Row 2: invalid instrument with new note (IT)
	 *
	 * When a new invalid instrument and a new note is set, IT
	 * keeps playing the current sample
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 2");

	xmp_play_frame(opaque);

	/* Row 3: note without ins continues after invalid ins (IT) */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
		 SET_VOL, SET_PAN, INS_0_FADE, "row 3");

	xmp_play_frame(opaque);

	/* Row 4 */
	xmp_play_frame(opaque);
	check_new(xc, vi, KEY_C5, INS_0,
		  INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 4");

	xmp_play_frame(opaque);

	/* Row 5: invalid subinstrument with new note (IT)
	 *
	 * An invalid subinstrument of a valid instrument continues.
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, -1 /* FIXME: desync, which should it be? */,
		 INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_1_FADE, "row 5");

	xmp_play_frame(opaque);

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
