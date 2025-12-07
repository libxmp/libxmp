/* Common functions for the read event test_new_note_*,
 * test_no_note_*, and test_porta_* regression tests. */

#include "read_event_common.h"

void create_read_event_test_module(struct context_data *ctx, int speed)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi;

	create_simple_module(ctx, 2, 2);

	mod->spd = speed;
	libxmp_scan_sequences(ctx); /* rescan for new speed */

	/* Instrument 1: activate second subinstrument */
	xxi = &mod->xxi[0];
	xxi->map[KEY_B5 - 1].ins = 1;

	set_instrument_volume(ctx, 0, 0, INS_0_SUB_0_VOL);
	set_instrument_volume(ctx, 0, 1, INS_0_SUB_1_VOL);
	set_instrument_panning(ctx, 0, 0, INS_0_SUB_0_PAN);
	set_instrument_panning(ctx, 0, 1, INS_0_SUB_1_PAN);
	set_instrument_fadeout(ctx, 0, INS_0_FADE);

	/* Instrument 2 */
	set_instrument_volume(ctx, 1, 0, INS_1_SUB_0_VOL);
	set_instrument_panning(ctx, 1, 0, INS_1_SUB_0_PAN);
	set_instrument_fadeout(ctx, 1, INS_1_FADE);
}

void check_active(struct channel_data *xc, struct mixer_voice *vi,
		  int note, int ins, int vol, int pan, int ins_fade,
		  const char *desc)
{
	char msg[256];

	snprintf(msg, sizeof(msg), "%s: mixer voice not active", desc);
	fail_unless(vi->chn >= 0, msg);

	snprintf(msg, sizeof(msg), "%s: note (xc:%d vi:%d) != expected %d",
		 desc, xc->note, vi->note, note - 1);
	fail_unless(xc->note == note - 1, msg);
	fail_unless(vi->note == note - 1, msg);

	if (ins >= 1) {
		snprintf(msg, sizeof(msg), "%s: ins (xc:%d vi:%d) != expected %d",
			 desc, xc->ins, vi->ins, ins - 1);
		fail_unless(xc->ins == ins - 1, msg);
		fail_unless(vi->ins == ins - 1, msg);
	}

	if (vol >= 0) {
		snprintf(msg, sizeof(msg), "%s: volume (xc:%d vi:%d) != expected %d",
			 desc, xc->volume, vi->vol, vol);
		fail_unless(xc->volume == vol, msg);
		fail_unless(vi->vol == (vol << 4), msg);
	}

	if (pan >= 0) {
		snprintf(msg, sizeof(msg), "%s: pan (xc:%d vi:%d) != expected %d",
			 desc, xc->pan.val, vi->pan + 0x80, pan);
		fail_unless(xc->pan.val == pan, msg);
		fail_unless(vi->pan + 0x80 == pan, msg);
	}

	if (ins_fade >= 0) {
		snprintf(msg, sizeof(msg), "%s: fade rate %d != expected %d",
			 desc, xc->ins_fade, ins_fade);
		fail_unless(xc->ins_fade == ins_fade, msg);
	}
}

void check_new(struct channel_data *xc, struct mixer_voice *vi,
	       int note, int ins, int vol, int pan, int ins_fade,
	       const char *desc)
{
	char msg[256];

	check_active(xc, vi, note, ins, vol, pan, ins_fade, desc);

	/* New notes have an initial mixer sample position of 0. */
	snprintf(msg, sizeof(msg), "%s: mixer sample pos %d != expected 0",
		 desc, vi->pos0);
	fail_unless(vi->pos0 == 0, msg);
}

void check_on(struct channel_data *xc, struct mixer_voice *vi,
	      int note, int ins, int vol, int pan, int ins_fade,
	      const char *desc)
{
	char msg[256];

	check_active(xc, vi, note, ins, vol, pan, ins_fade, desc);

	/* Note should not be new, but rather continued from another line. */
	snprintf(msg, sizeof(msg), "%s: mixer sample pos %d != expected !0",
		 desc, vi->pos0);
	fail_unless(vi->pos0 != 0, msg);
}

void check_off(struct channel_data *xc, struct mixer_voice *vi,
	       const char *desc)
{
	char msg[256];

	/* Fully off mixer channels should be unmapped and have volume 0. */
	snprintf(msg, sizeof(msg), "%s: mixer voice is active", desc);
	fail_unless(vi->chn < 0, msg);

	snprintf(msg, sizeof(msg), "%s: mixer voice volume %d != 0",
		 desc, vi->vol);
	fail_unless(vi->vol == 0, msg);
}
