#include "read_event_common.h"

/*
Case 3: Tone portamento

  Instrument -> None    Same    Valid   Inval
PT1.1           Cont            NewVol?
PT1.3           Cont    NewVol  NewVol* Cut
PT2.3           Cont    NewVol  NewVol* Cut
PT3.15          Cont    NewVol  NewVol  Cut     <= "Standard"
PT3.61          Cont    NewVol  NewVol  Cut     <=
PT4b2           Cont    NewVol  NewVol  Cut     <=
MED             Cont    NewVol  NewVol  Cut     <=
FT2             Cont    OldVol  OldVol  OldVol
ST3             Cont    NewVol  NewVol  Cont
IT(s)           Cont    NewVol  NewVol  Cont
IT(i) @         Cont    NewVol  NewVol  Cont
DT32            Cont    NewVol  NewVol  Cut

*/

TEST(test_porta_same_ins_st3)
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
	new_event(ctx, 0, 0, 0, KEY_C5, INS_0, 0, 0x00, 0, 0, 0);
	new_event(ctx, 0, 1, 0, 0,      0,     0, FX_VOLSET, SET_VOL,
						  FX_SETPAN, SET_PAN);
	new_event(ctx, 0, 2, 0, KEY_D4, INS_0, 0, FX_TONEPORTA, 4, 0, 0);
	set_quirk(ctx, QUIRKS_ST3, READ_EVENT_ST3);

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

	/* Row 2: same instrument with tone portamento (ST3)
	 *
	 * When the same instrument as the current instrument is played
	 * with tone portamento, ST3 keeps playing the current sample but
	 * sets the volume to the instrument's default volume.
	 */
	xmp_play_frame(opaque);
	check_on(xc, vi, KEY_C5, INS_0,
	         INS_0_SUB_0_VOL, INS_0_SUB_0_PAN, INS_0_FADE, "row 2");

	xmp_play_frame(opaque);

	xmp_release_module(opaque);
	xmp_free_context(opaque);
}
END_TEST
