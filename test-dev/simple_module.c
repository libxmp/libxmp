/* Creates a simple module */

#include "test.h"
#include "../src/common.h"
#include "../src/loaders/loader.h"

void create_simple_module(struct context_data *ctx, int ins, int pat)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi;
	struct xmp_subinstrument *sub;
	struct xmp_sample *xxs;
	int i, j;

	libxmp_load_prologue(ctx);

	/* Create module */

	mod->len = 2;
	mod->pat = pat;
	mod->ins = ins;
	mod->chn = 4;
	mod->trk = mod->pat * mod->chn;
	mod->smp = mod->ins;
	mod->xxo[0] = 0;
	mod->xxo[1] = 1;

	libxmp_init_pattern(mod);

	for (i = 0; i < mod->pat; i++) {
		libxmp_alloc_pattern_tracks(mod, i, 64);
	}

	libxmp_init_instrument(m);

	for (i = 0; i < mod->ins; i++) {
		xxi = &mod->xxi[i];

		/* Give each instrument two subinstruments mapped to the
		 * same sample. By default, only the first will be used. */
		xxi->nsm = 2;
		libxmp_alloc_subinstrument(mod, i, 2);

		for (j = 0; j < xxi->nsm; j++) {
			sub = &xxi->sub[j];
			sub->pan = -1;
			sub->vol = 0x40;
			sub->sid = i;
		}

		xxs = &mod->xxs[i];
		xxs->len = 10000;
		xxs->lps = 0;
		xxs->lpe = 10000;
		xxs->flg = XMP_SAMPLE_LOOP;
		xxs->data = (unsigned char *) calloc(1, 11000);
		xxs->data += 4;
	}

	/* End of module creation */

	libxmp_load_epilogue(ctx);
	libxmp_prepare_scan(ctx);
	libxmp_scan_sequences(ctx);

	ctx->state = XMP_STATE_LOADED;
}

void new_event(struct context_data *ctx, int pat, int row, int chn, int note, int ins, int vol, int fxt, int fxp, int f2t, int f2p)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_event *e;
	int track;

	track = mod->xxp[pat]->index[chn];
	e = &mod->xxt[track]->event[row];

	e->note = note;
	e->ins = ins;
	e->vol = vol;
	e->fxt = fxt;
	e->fxp = fxp;
	e->f2t = f2t;
	e->f2p = f2p;
}

void set_order(struct context_data *ctx, int pos, int pat)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxo[pos] = pat;
	mod->len = pos + 1;
}

void set_instrument_volume(struct context_data *ctx, int ins, int sub, int vol)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].sub[sub].vol = vol;
}

void set_instrument_panning(struct context_data *ctx, int ins, int sub, int pan)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].sub[sub].pan = pan;
}

void set_instrument_nna(struct context_data *ctx, int ins, int sub,
			int nna, int dct, int dca)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].sub[sub].nna = nna;
	mod->xxi[ins].sub[sub].dct = dct;
	mod->xxi[ins].sub[sub].dca = dca;
}

void set_instrument_envelope(struct context_data *ctx, int ins, int node,
			     int x, int y)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].aei.data[node * 2] = x;
	mod->xxi[ins].aei.data[node * 2 + 1] = y;

	mod->xxi[ins].aei.npt = node + 1;
	mod->xxi[ins].aei.flg |= XMP_ENVELOPE_ON;
}

void set_instrument_envelope_loop(struct context_data *ctx, int ins, int lps, int lpe)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].aei.lps = lps;
	mod->xxi[ins].aei.lpe = lpe;
	mod->xxi[ins].aei.flg |= XMP_ENVELOPE_LOOP;
}

void set_instrument_envelope_sus(struct context_data *ctx, int ins, int sus)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].aei.sus = sus;
	mod->xxi[ins].aei.sue = sus;
	mod->xxi[ins].aei.flg |= XMP_ENVELOPE_SUS | XMP_ENVELOPE_SLOOP;
}

void set_instrument_fadeout(struct context_data *ctx, int ins, int fade)
{
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;

	mod->xxi[ins].rls = fade;
}

void set_period_type(struct context_data *ctx, int type)
{
	struct module_data *m = &ctx->m;

	m->period_type = type;
}

void set_quirk(struct context_data *ctx, int quirk, int read_mode)
{
	struct module_data *m = &ctx->m;

	m->quirk |= quirk;
	m->read_event_type = read_mode;
}

void reset_quirk(struct context_data *ctx, int quirk)
{
	struct module_data *m = &ctx->m;

	m->quirk &= ~quirk;
}
