/* Extended Module Player
 * Copyright (C) 1996-2025 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "common.h"
#include "player.h"
#include "virtual.h"
#include "effects.h"
#include "extras.h"
#include "med_extras.h"
#include "loaders/med.h"

#ifdef __SUNPRO_C
#pragma error_messages (off,E_STATEMENT_NOT_REACHED)
#endif

/* Commands in the volume and waveform sequence table:
 *
 *	Cmd	Vol	Wave	Action
 *
 *	0xff	    END		End sequence
 *	0xfe	    JMP		Jump
 *	0xfd	 -	ARE	End arpeggio definition
 *	0xfc	 -	ARP	Begin arpeggio definition
 *	0xfb	    HLT		Halt
 *	0xfa	JWS	JVS	Jump waveform/volume sequence
 *	0xf9	     -		-
 *	0xf8	     -		-
 *	0xf7	 -	VWF	Set vibrato waveform
 *	0xf6	EST	RES	?/reset pitch
 *	0xf5	EN2	VBS	Looping envelope/set vibrato speed
 *	0xf4	EN1	VBD	One shot envelope/set vibrato depth
 *	0xf3	    CHU		Change volume/pitch up speed
 *	0xf2	    CHD		Change volume/pitch down speed
 *	0xf1	    WAI		Wait
 *	0xf0	    SPD		Set speed
 */

#define VT ((ce->vp >= 0 && ce->vp < ie->vtlen) ? me->vol_table[xc->ins][ce->vp++] : 0xff)
#define WT ((ce->wp >= 0 && ce->wp < ie->wtlen) ? me->wav_table[xc->ins][ce->wp++] : 0xff)
#define VT_SKIP ce->vp++
#define WT_SKIP ce->wp++

#define ARP(idx) ((idx) < ie->wtlen ? me->wav_table[xc->ins][(idx)] : 0xfd)


static const int sine[32] = {
	   0,  49,  97, 141, 180, 212, 235, 250,
	 255, 250, 235, 212, 180, 141,  97,  49,
	   0, -49, -97,-141,-180,-212,-235,-250,
	-255,-250,-235,-212,-180,-141, -97, -49
};

int libxmp_med_change_period(struct context_data *ctx, struct channel_data *xc)
{
	struct med_channel_extras *ce = (struct med_channel_extras *)xc->extra;
	int vib;

	/* Vibrato */

#if 0
	if (ce->vib_wf >= xxi[xc->ins].nsm)	/* invalid waveform */
		return 0;

	if (xxs[xxi[xc->ins][ce->vib_wf].sid].len != 32)
		return 0;
#endif

	/* FIXME: always using sine waveform */

	vib = (sine[ce->vib_idx >> 5] * ce->vib_depth) >> 10;
	ce->vib_idx += ce->vib_speed;
	ce->vib_idx %= (32 << 5);

	return vib;
}


int libxmp_med_linear_bend(struct context_data *ctx, struct channel_data *xc)
{
	struct module_data *m = &ctx->m;
	struct xmp_instrument *xxi = &m->mod.xxi[xc->ins];
	struct med_module_extras *me = (struct med_module_extras *)m->extra;
	struct med_channel_extras *ce = (struct med_channel_extras *)xc->extra;
	struct med_instrument_extras *ie = MED_INSTRUMENT_EXTRAS(*xxi);
	int arp;

	/* Arpeggio */

	if (ce->arp == 0)
		return 0;

	if (ARP(ce->arp) == 0xfd) /* empty arpeggio */
		return 0;

	arp = ARP(ce->aidx);
	if (arp == 0xfd) {
		ce->aidx = ce->arp;
		arp = ARP(ce->aidx);
	}
	ce->aidx++;

	return (100 << 7) * arp;
}


/* Hold/decay support functions.
 *
 * Hold/decay should only occur if the channel has a configured hold
 * value (from instrument or an initial hold/decay command). If the
 * decay is 0, the note volume is immediately set to 0 when hold ends.
 *
 * "Hold symbols" (instrument number with no note) sustain hold on
 * occuring on the line PRIOR to them. Hold countdown begins on the
 * last line with a hold symbol if present, otherwise immediately.
 *
 * TODO: MED bug: note delay doesn't delay the hold? It seems to play a
 * fragment of the prior note before the new note starts in some cases.
 * TODO: pre-OctaMED 3.00 FF2 note delay breaks hold entirely?
 */

void libxmp_med_set_hold_decay(struct context_data *ctx,
			       struct channel_data *xc, int hold, int decay)
{
	if (!HAS_MED_CHANNEL_EXTRAS(*xc)) {
		return;
	}
	if (hold <= 0) {
		hold = decay = -1;
	}
	MED_CHANNEL_EXTRAS(*xc)->hold_active = (hold > 0);
	MED_CHANNEL_EXTRAS(*xc)->hold_count = hold;
	MED_CHANNEL_EXTRAS(*xc)->decay_value = decay;
}

/* OctaMED 3.00 onward: each individual retrigger increases the hold counter
 * by the CURRENT TICK NUMBER if decay hasn't started, allowing very long holds.
 * Presumably this was intended to be the retrigger delay instead.
 *
 * Example: Aftab Hussain/deep cover.mmd3 pos 36/block 30 ch.2 rows 60-64
 * uses retrigger hold extension for a snare roll, but it doesn't rely
 * on the extra long hold length bug.
 */
void libxmp_med_hold_retrigger(struct context_data *ctx, struct channel_data *xc)
{
	struct module_data *m = &ctx->m;

	if (!HAS_MED_CHANNEL_EXTRAS(*xc)) {
		return;
	}
	if (MED_MODULE_EXTRAS(*m)->tracker_version >= MED_VER_OCTAMED_300 &&
	    MED_CHANNEL_EXTRAS(*xc)->hold_active) {
		MED_CHANNEL_EXTRAS(*xc)->hold_count += ctx->p.frame;
	}
}

/**
 * Hack to get the next event in the pattern (for implementing hold symbols).
 * This may not work as intended for injected events.
 */
static struct xmp_event *get_next_event(struct context_data *ctx, int chn)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	int pat = mod->xxo[p->ord];
	int num_rows = mod->xxt[TRACK_NUM(pat, chn)]->rows;

	if (p->row + 1 >= num_rows) {
		return NULL;
	}
	return &EVENT(pat, chn, p->row + 1);
}

/* Should be called from read_event_med only. */
void libxmp_med_check_hold_symbol(struct context_data *ctx,
				  struct channel_data *xc, int chn)
{
	struct xmp_event *e;

	MED_CHANNEL_EXTRAS(*xc)->hold_sustained = 0;

	/* Hold/decay added in MED 3.00 / OctaMED 1.00 */
	if (MED_MODULE_EXTRAS(ctx->m)->tracker_version < MED_VER_300)
		return;
	if (MED_CHANNEL_EXTRAS(*xc)->hold_count <= 0)
		return;
	if ((e = get_next_event(ctx, chn)) == NULL)
		return;

	/* No note + ins -> sustain hold. */
	if (!IS_VALID_NOTE((int)e->note - 1) && e->ins != 0) {
		MED_CHANNEL_EXTRAS(*xc)->hold_sustained = 1;
		return;
	}

	/* Note + toneporta (3xx only, NOT 5xy) -> sustain hold. */
	if (IS_VALID_NOTE((int)e->note - 1) &&
	    (e->fxt == FX_TONEPORTA || e->f2t == FX_TONEPORTA)) {
		MED_CHANNEL_EXTRAS(*xc)->hold_sustained = 1;
	}
}

static void med_tick_hold_decay(struct context_data *ctx,
				struct channel_data *xc,
				struct med_module_extras *me,
				struct med_channel_extras *ce)
{
	struct module_data *m = &ctx->m;
	struct player_data *p = &ctx->p;

	if (ce->hold_count == 0) {
		/* Decay 0 = instant decay. */
		int dec = ce->decay_value ? ce->decay_value : xc->volume;

		xc->volume -= dec;
		CLAMP(xc->volume, 0, m->volbase);

		if (xc->volume == 0) {
			/* End hold/decay once volume 0 is reached. */
			ce->hold_count = ce->decay_value = -1;
		}
		/* Hold can't be sustained/increased after decay starts(?). */
		ce->hold_active = 0;
	}

	/* From OctaMED 3.00 through Soundstudio v1, pattern delay (1Exx) on
	 * a line with hold sustained will only sustain the hold for the first
	 * execution of the row. In Soundstudio v2, the whole row sustains. */
	if (ce->hold_sustained && me->tracker_version < MED_VER_OCTAMED_SS_2 &&
	    p->frame >= p->speed) {
		ce->hold_sustained = 0;
	}

	/* Hold is sustained if the NEXT row contains a sustain event.
	 * MED probably actually decrements hold after effects processing;
	 * using ce->hold_active as a hack to work around this. */
	if (ce->hold_count > 0 && !ce->hold_sustained) {
		ce->hold_count--;
	}
}


void libxmp_med_play_extras(struct context_data *ctx, struct channel_data *xc, int chn)
{
	struct module_data *m = &ctx->m;
	struct player_data *p = &ctx->p;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi = &m->mod.xxi[xc->ins];
	struct med_module_extras *me;
	struct med_channel_extras *ce;
	struct med_instrument_extras *ie;
	int b, jws = 0, jvs = 0, loop;
	int temp;

	if (!HAS_MED_MODULE_EXTRAS(*m))
		return;

	me = (struct med_module_extras *)m->extra;
	ce = (struct med_channel_extras *)xc->extra;
	ie = MED_INSTRUMENT_EXTRAS(*xxi);

	/* Handle hold/decay */

	med_tick_hold_decay(ctx, xc, me, ce);

	/* Handle synth */

	if (me->vol_table[xc->ins] == NULL || me->wav_table[xc->ins] == NULL) {
		ce->volume = 64;  /* we need this in extras_get_volume() */
		return;
	}

	if (p->frame == 0 && TEST(NEW_NOTE)) {
		ce->period = xc->period;
		if (TEST(NEW_INS)) {
			ce->arp = ce->aidx = 0;
			ce->vp = ce->vc = ce->vw = 0;
			ce->wp = ce->wc = ce->ww = 0;
			ce->env_wav = -1;
			ce->env_idx = 0;
			ce->flags &= ~MED_SYNTH_ENV_LOOP;
			ce->vv = 0;
			ce->wv = 0;
			ce->vs = ie->vts;
			ce->ws = ie->wts;
		}
	}

	if (ce->vs > 0 && ce->vc-- == 0) {
		ce->vc = ce->vs - 1;

		if (ce->vw > 0) {
			ce->vw--;
			goto skip_vol;
		}

		loop = jws = 0;

		/* Volume commands */

	    next_vt:
		switch (b = VT) {
		case 0xff:	/* END */
		case 0xfb:	/* HLT */
			ce->vp--;
			break;
		case 0xfe:	/* JMP */
			if (loop)	/* avoid infinite loop */
				break;
			temp = VT;
			ce->vp = temp;
			loop = 1;
			goto next_vt;
		case 0xfa:	/* JWS */
			jws = VT;
			break;
		case 0xf5:	/* EN2 */
			ce->env_wav = VT;
			ce->flags |= MED_SYNTH_ENV_LOOP;
			break;
		case 0xf4:	/* EN1 */
			ce->env_wav = VT;
			break;
		case 0xf3:	/* CHU */
			ce->vv = VT;
			break;
		case 0xf2:	/* CHD */
			ce->vv = -VT;
			break;
		case 0xf1:	/* WAI */
			ce->vw = VT;
			break;
		case 0xf0:	/* SPD */
			ce->vs = VT;
			break;
		default:
			if (b >= 0x00 && b <= 0x40)
				ce->volume = b;
		}

	    skip_vol:
		/* volume envelope */
		if (ce->env_wav >= 0 && ce->env_wav < xxi->nsm) {
			int sid = xxi->sub[ce->env_wav].sid;
			struct xmp_sample *xxs = &mod->xxs[sid];
			if (xxs->len == 0x80) {		/* sanity check */
				ce->volume = ((int8)xxs->data[ce->env_idx] + 0x80) >> 2;
				ce->env_idx++;

				if (ce->env_idx >= 0x80) {
					if (~ce->flags & MED_SYNTH_ENV_LOOP) {
						ce->env_wav = -1;
					}
					ce->env_idx = 0;
				}
			}
		}

		ce->volume += ce->vv;
		CLAMP(ce->volume, 0, 64);
	}

	if (ce->ws > 0 && ce->wc-- == 0) {
		ce->wc = ce->ws - 1;

		if (ce->ww > 0) {
			ce->ww--;
			goto skip_wav;
		}

		loop = jvs = 0;

		/* Waveform commands */

	    next_wt:
		switch (b = WT) {
		case 0xff:	/* END */
		case 0xfb:	/* HLT */
			ce->wp--;
			break;
		case 0xfe:	/* JMP */
			if (loop)	/* avoid infinite loop */
				break;
			temp = WT;
			if (temp == 0xff) {	/* handle JMP END case */
				ce->wp--;	/* see lepeltheme ins 0x02 */
				break;
			}
			ce->wp = temp;
			loop = 1;
			goto next_wt;
		case 0xfd:	/* ARE */
			break;
		case 0xfc:	/* ARP */
			ce->arp = ce->aidx = ce->wp++;
			while (b != 0xfd && b != 0xff) b = WT;
			break;
		case 0xfa:	/* JVS */
			jvs = WT;
			break;
		case 0xf7:	/* VWF */
			ce->vwf = WT;
			break;
		case 0xf6:	/* RES */
			xc->period = ce->period;
			break;
		case 0xf5:	/* VBS */
			ce->vib_speed = WT;
			break;
		case 0xf4:	/* VBD */
			ce->vib_depth = WT;
			break;
		case 0xf3:	/* CHU */
			ce->wv = -WT;
			break;
		case 0xf2:	/* CHD */
			ce->wv = WT;
			break;
		case 0xf1:	/* WAI */
			ce->ww = WT;
			break;
		case 0xf0:	/* SPD */
			ce->ws = WT;
			break;
		default:
			if (b < xxi->nsm && xxi->sub[b].sid != xc->smp) {
				xc->smp = xxi->sub[b].sid;
				libxmp_virt_setsmp(ctx, chn, xc->smp);
			}
		}

	    skip_wav:
		xc->period += ce->wv;
	}

	if (jws) {
		ce->wp = jws;
		/* jws = 0; */
	}

	if (jvs) {
		ce->vp = jvs;
		/* jvs = 0; */
	}
}

int libxmp_med_new_instrument_extras(struct xmp_instrument *xxi)
{
	xxi->extra = calloc (1, sizeof(struct med_instrument_extras));
	if (xxi->extra == NULL)
		return -1;
	MED_INSTRUMENT_EXTRAS((*xxi))->magic = MED_EXTRAS_MAGIC;

	return 0;
}

int libxmp_med_new_channel_extras(struct channel_data *xc)
{
	xc->extra = calloc(1, sizeof(struct med_channel_extras));
	if (xc->extra == NULL)
		return -1;
	MED_CHANNEL_EXTRAS((*xc))->magic = MED_EXTRAS_MAGIC;
	MED_CHANNEL_EXTRAS((*xc))->hold_count = -1;

	return 0;
}

void libxmp_med_reset_channel_extras(struct channel_data *xc)
{
	memset((char *)xc->extra + 4, 0, sizeof(struct med_channel_extras) - 4);
}

void libxmp_med_release_channel_extras(struct channel_data *xc)
{
	free(xc->extra);
	xc->extra = NULL;
}

int libxmp_med_new_module_extras(struct module_data *m)
{
	struct med_module_extras *me;
	struct xmp_module *mod = &m->mod;

	m->extra = calloc(1, sizeof(struct med_module_extras));
	if (m->extra == NULL)
		return -1;
	MED_MODULE_EXTRAS((*m))->magic = MED_EXTRAS_MAGIC;

	me = (struct med_module_extras *)m->extra;

	me->vol_table = (uint8 **) calloc(mod->ins, sizeof(uint8 *));
	if (me->vol_table == NULL)
		return -1;
	me->wav_table = (uint8 **) calloc(mod->ins, sizeof(uint8 *));
	if (me->wav_table == NULL)
		return -1;

	return 0;
}

void libxmp_med_release_module_extras(struct module_data *m)
{
	struct med_module_extras *me;
	struct xmp_module *mod = &m->mod;
	int i;

	me = (struct med_module_extras *)m->extra;

	if (me->vol_table) {
		for (i = 0; i < mod->ins; i++)
			free(me->vol_table[i]);
		free(me->vol_table);
	}

	if (me->wav_table) {
		for (i = 0; i < mod->ins; i++)
			free(me->wav_table[i]);
		free(me->wav_table);
	}

	free(m->extra);
	m->extra = NULL;
}

void libxmp_med_extras_process_fx(struct context_data *ctx, struct channel_data *xc,
		int chn, uint8 note, uint8 ins, uint8 fxt, uint8 fxp, int fnum)
{
	struct xmp_module *mod = &ctx->m.mod;

	switch (fxt) {
	case FX_MED_HOLD:
		/* Command 08 is only valid beside a note+ins. */
		if (IS_VALID_NOTE((int)note - 1) &&
		    IS_VALID_INSTRUMENT((int)ins - 1)) {
			libxmp_med_set_hold_decay(ctx, xc, LSN(fxp), MSN(fxp));
		}
		break;
	}
}
