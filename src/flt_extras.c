/* Extended Module Player
 * Copyright (C) 1996-2026 Claudio Matsuoka and Hipolito Carraro Jr
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
#include "flt_extras.h"

/* StarTrekker AM instrument extras.
 *
 * Since the envelope overrides channel volume instead of multiplying against
 * it, and some modules (Epsilon (ES)/frankie.mod, WOTW/intro 12.mod) rely
 * on this for some reason, extras are used instead of XM envelopes.
 *
 * TODO: AM vibrato could probably go here.
 */

static void libxmp_flt_tick_envelope(struct flt_channel_extras *ce, int target,
				     int rate, enum flt_env_stage next_stage)
{
	int x;

	if (target > ce->volume) {
		x = ce->volume + rate;
		if (x > target) x = target;
	} else {
		x = ce->volume - rate;
		if (x < target) x = target;
	}

	ce->volume = x;
	if (x == target) {
		ce->env_stage = next_stage;
	}
}

void libxmp_flt_play_extras(struct context_data *ctx, struct channel_data *xc, int chn)
{
	struct player_data *p = &ctx->p;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi = &mod->xxi[xc->ins];
	struct flt_channel_extras *ce = FLT_CHANNEL_EXTRAS(*xc);
	struct flt_instrument_extras *ie = FLT_INSTRUMENT_EXTRAS(*xxi);

	if (p->frame == 0) {
		if (TEST(NEW_NOTE) && !TEST(TONEPORTA)) {
			/* Note with or without ins #, no toneporta -> reset. */
			ce->volume = ie->l0;
			ce->sustain = ie->st;
			ce->env_stage = FLT_ATTACK_1;
		} else if (TEST(NEW_INS)) {
			/* Ins # without note cuts (regardless of # or toneporta).
			 * Ins # with note and toneporta also cuts(?!). */
			ce->volume = 0;
			ce->env_stage = FLT_RELEASE;
		} else if (TEST(NEW_NOTE) && TEST(TONEPORTA)) {
			/* StarTrekker forgot to adjust the toneporta target by
			 * FQ, so reverse transpose (flt_am_period_fall.mod). */
			xc->porta.target /= (double)(1 << ie->fq);
			xc->porta.dir = xc->period < xc->porta.target ? 1 : -1;
		}
	}

	switch (ce->env_stage) {
	case FLT_ATTACK_1:
		libxmp_flt_tick_envelope(ce, ie->a1l, ie->a1s, FLT_ATTACK_2);
		break;
	case FLT_ATTACK_2:
		libxmp_flt_tick_envelope(ce, ie->a2l, ie->a2s, FLT_DECAY);
		break;
	case FLT_DECAY:
		libxmp_flt_tick_envelope(ce, ie->sl,  ie->ds,  FLT_SUSTAIN);
		break;
	case FLT_SUSTAIN:
		/* Total duration is ST + 1 ticks. */
		if (ce->sustain > 0) {
			ce->sustain--;
		} else {
			ce->env_stage = FLT_RELEASE;
		}
		break;
	case FLT_RELEASE:
		libxmp_flt_tick_envelope(ce, 0,       ie->rs,  FLT_RELEASE);
		break;
	}

	/* Add directly to the period--this influences things like toneporta. */
	xc->period += ie->p_fall;
}

int libxmp_flt_new_instrument_extras(struct xmp_instrument *xxi)
{
	xxi->extra = calloc(1, sizeof(struct flt_instrument_extras));
	if (xxi->extra == NULL)
		return -1;
	FLT_INSTRUMENT_EXTRAS((*xxi))->magic = FLT_EXTRAS_MAGIC;
	return 0;
}

int libxmp_flt_new_channel_extras(struct channel_data *xc)
{
	xc->extra = calloc(1, sizeof(struct flt_channel_extras));
	if (xc->extra == NULL)
		return -1;
	FLT_CHANNEL_EXTRAS((*xc))->magic = FLT_EXTRAS_MAGIC;
	return 0;
}

void libxmp_flt_reset_channel_extras(struct channel_data *xc)
{
	memset((char *)xc->extra + 4, 0, sizeof(struct flt_channel_extras) - 4);
}

void libxmp_flt_release_channel_extras(struct channel_data *xc)
{
	free(xc->extra);
	xc->extra = NULL;
}

int libxmp_flt_new_module_extras(struct module_data *m)
{
	m->extra = calloc(1, sizeof(struct flt_module_extras));
	if (m->extra == NULL)
		return -1;
	FLT_MODULE_EXTRAS((*m))->magic = FLT_EXTRAS_MAGIC;
	return 0;
}

void libxmp_flt_release_module_extras(struct module_data *m)
{
	free(m->extra);
	m->extra = NULL;
}
