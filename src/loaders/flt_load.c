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

#include "loader.h"
#include "mod.h"
#include "../flt_extras.h"
#include "../path.h"
#include "../period.h"
#include "../rng.h"

static int flt_test(HIO_HANDLE *, char *, const int);
static int flt_load(struct module_data *, HIO_HANDLE *, const int);

const struct format_loader libxmp_loader_flt = {
	"Startrekker",
	flt_test,
	flt_load
};

static int flt_test(HIO_HANDLE * f, char *t, const int start)
{
	char buf[4];

	hio_seek(f, start + 1080, SEEK_SET);
	if (hio_read(buf, 1, 4, f) < 4)
		return -1;

	/* Also RASP? */
	if (memcmp(buf, "FLT", 3) && memcmp(buf, "EXO", 3))
		return -1;

	if (buf[3] != '4' && buf[3] != '8' && buf[3] != 'M')
		return -1;

	hio_seek(f, start + 0, SEEK_SET);
	libxmp_read_title(f, t, 20);

	return 0;
}

/* Waveforms from the Startrekker 1.2 AM synth replayer code */

static const int8 am_waveform[3][32] = {
	{    0,   25,   49,   71,   90,  106,  117,  125,	/* Sine */
	   127,  125,  117,  106,   90,   71,   49,   25,
	     0,  -25,  -49,  -71,  -90, -106, -117, -125,
	  -127, -125, -117, -106,  -90,  -71,  -49,  -25
	},

	{ -128, -120, -112, -104,  -96,  -88,  -80,  -72,	/* Ramp */
	   -64,  -56,  -48,  -40,  -32,  -24,  -16,   -8,
	     0,    8,   16,   24,   32,   40,   48,   56,
	    64,   72,   80,   88,   96,  104,  112,  120
	},

	{ -128, -128, -128, -128, -128, -128, -128, -128,	/* Square */
	  -128, -128, -128, -128, -128, -128, -128, -128,
	   127,  127,  127,  127,  127,  127,  127,  127,
	   127,  127,  127,  127,  127,  127,  127,  127
	}
};

struct am_instrument {
	uint16 l0;		/* start amplitude */
	uint16 a1l;		/* attack level */
	uint16 a1s;		/* attack speed */
	uint16 a2l;		/* secondary attack level */
	uint16 a2s;		/* secondary attack speed */
	uint16 sl;		/* sustain level */
	uint16 ds;		/* decay speed */
	uint16 st;		/* sustain time */
	uint16 rs;		/* release speed */
	int16 wf;		/* waveform */
	int16 p_fall;		/* "pitch" fall (add to period each tick) */
	int16 v_amp;		/* vibrato amplitude */
	int16 v_spd;		/* vibrato speed */
	int16 fq;		/* base frequency */
};

static int is_am_instrument(HIO_HANDLE *nt, int i)
{
	uint8 buf[28];

	hio_seek(nt, 144 + i * 120, SEEK_SET);
	if (hio_read(buf, 1, sizeof(buf), nt) < sizeof(buf))
		return 0;
	if (memcmp(buf, "AM", 2))
		return 0;
	if (readmem16b(buf + 26) /* WF */ > 3)
		return 0;

	return 1;
}

#if 0
/*
 * AM synth envelope parameters based on the Startrekker 1.2 docs
 *
 * L0    Start amplitude for the envelope
 * A1L   Attack level
 * A1S   The speed that the amplitude changes to the attack level, $1
 *       is slow and $40 is fast.
 * A2L   Secondary attack level, for those who likes envelopes...
 * A2S   Secondary attack speed.
 * DS    The speed that the amplitude decays down to the:
 * SL    Sustain level. There is remains for the time set by the
 * ST    Sustain time.
 * RS    Release speed. The speed that the amplitude falls from ST to 0.
 *
 * Keeping this (now working) conversion routine as intact deadcode, as it
 * may be useful if libxmp ever needs to convert StarTrekker envelopes again.
 */

#define AM_SET_ENVELOPE(time, lv) do { \
	env->data[pos]     = (time); \
	env->data[pos + 1] = (lv) / 4; \
	env->npt++; \
	pos += 2; \
} while(0)

#define AM_ADD_ENVELOPE(dur, lv) do { \
	int prev_time = env->data[pos - 2]; \
	AM_SET_ENVELOPE(prev_time + (dur), (lv)); \
} while(0)

/* Speed is linear and is added/subtracted once every tick before
 * checking for the target level, making the duration of stage n:
 *
 * Duration[n] = max(ceil(abs(L[n] - L[n-1]) / S[n]), 1)
 *
 * A speed of 0 causes the envelope to stop at the level of the
 * previous stage (unless the target level is the same as the
 * previous level). This can be simulated with a sustain point.
 * (Add a new point for sustain to guarantee there at least 2 points.)
 */
#define AM_STAGE_DURATION(dur, prev_lv, lv, rate) do { \
	int a; \
	if ((rate) == 0 && (lv) != (prev_lv)) { \
		env->flg |= XMP_ENVELOPE_SUS; \
		env->sus = env->sue = env->npt; \
		AM_ADD_ENVELOPE(1, (prev_lv)); \
		return; \
	} \
	a = ((lv) > (prev_lv)) ? (lv) - (prev_lv) : (prev_lv) - (lv); \
	if ((rate) > 0 && a > 0) { \
		(dur) = (a + (rate) - 1) / (rate); \
	} else { \
		(dur) = 1; \
	} \
} while(0)

static void convert_am_envelope(struct xmp_envelope *env, const struct am_instrument *am)
{
	int adjusted_l0;
	int duration;
	int pos = 0;

	env->flg |= XMP_ENVELOPE_ON;
	env->npt = 0;

	/* Attack 1
	 * This stage begins immediately on tick 0. To convert this to an XM/IT
	 * envelope, one step needs to be applied to the initial level.
	 * If attack 1 completes on this tick, don't add its envelope point. */
	if (am->a1l > am->l0) {
		adjusted_l0 = MIN(am->l0 + am->a1s, am->a1l);
	} else {
		adjusted_l0 = MAX(am->l0 - am->a1s, am->a1l);
	}
	AM_SET_ENVELOPE(0, adjusted_l0);
	if (adjusted_l0 != am->a1l) {
		AM_STAGE_DURATION(duration, adjusted_l0, am->a1l, am->a1s);
		AM_ADD_ENVELOPE(duration, am->a1l);
	}

	/* Attack 2 */
	AM_STAGE_DURATION(duration, am->a1l, am->a2l, am->a2s);
	AM_ADD_ENVELOPE(duration, am->a2l);

	/* Decay */
	AM_STAGE_DURATION(duration, am->a2l, am->sl, am->ds);
	AM_ADD_ENVELOPE(duration, am->sl);

	/* Sustain seems to check for termination before decrementing,
	 * meaning duration 0 takes 1 tick, 1 takes 2 ticks, etc. */
	AM_ADD_ENVELOPE(am->st + 1, am->sl);

	/* Release */
	AM_STAGE_DURATION(duration, am->sl, 0, am->rs);
	AM_ADD_ENVELOPE(duration, 0);
}
#endif

static int read_am_instrument(struct module_data *m, HIO_HANDLE *nt, int i)
{
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi = &mod->xxi[i];
	struct xmp_sample *xxs = &mod->xxs[i];
	struct flt_instrument_extras *extra;
	struct am_instrument am;
	struct rng_state rng;
	const int8 *wave;
	uint8 buf[30];
	int8 am_noise[1024];

	memset(buf, 0, sizeof(buf));

	hio_seek(nt, 144 + i * 120 + 2 + 4, SEEK_SET);
	/* Allow partial/missing AM instruments (GTS/fa.worse face.mod). */
	if (hio_read(buf, 1, 30, nt) < 30) {
		D_(D_WARN "AM instrument %d is truncated or missing", i);
	}
	am.l0  = readmem16b(buf + 0);
	am.a1l = readmem16b(buf + 2);
	am.a1s = readmem16b(buf + 4);
	am.a2l = readmem16b(buf + 6);
	am.a2s = readmem16b(buf + 8);
	am.sl  = readmem16b(buf + 10);
	am.ds  = readmem16b(buf + 12);
	am.st  = readmem16b(buf + 14);
	am.rs  = readmem16b(buf + 18);
	am.wf  = readmem16b(buf + 20);
	am.p_fall = (int16) readmem16b(buf + 22);
	am.v_amp = readmem16b(buf + 24);
	am.v_spd = readmem16b(buf + 26);
	am.fq  = readmem16b(buf + 28);

#if 0
	printf
	    ("L0=%d A1L=%d A1S=%d A2L=%d A2S=%d SL=%d DS=%d ST=%d RS=%d WF=%d\n",
	     am.l0, am.a1l, am.a1s, am.a2l, am.a2s, am.sl, am.ds, am.st, am.rs,
	     am.wf);
#endif

	if (am.wf < 3) {
		xxs->len = 32;
		xxs->lps = 0;
		xxs->lpe = 32;
		wave = am_waveform[am.wf];
	} else {
		int j;

		xxs->len = 1024;
		xxs->lps = 0;
		xxs->lpe = 1024;

		libxmp_init_random(&rng);
		for (j = 0; j < 1024; j++)
			am_noise[j] = libxmp_get_random(&rng, 256);

		wave = am_noise;
	}

	xxs->flg = XMP_SAMPLE_LOOP;
	xxi->nsm = 1;
	xxi->sub[0].xpo = -12 * am.fq;
	xxi->sub[0].vwf = 0;
	xxi->sub[0].vde = am.v_amp * 4;
	xxi->sub[0].vra = am.v_spd;

	if (libxmp_flt_new_instrument_extras(xxi) < 0)
		return -1;

	extra = FLT_INSTRUMENT_EXTRAS(*xxi);
	extra->l0  = am.l0;
	extra->a1l = am.a1l;
	extra->a1s = am.a1s;
	extra->a2l = am.a2l;
	extra->a2s = am.a2s;
	extra->sl  = am.sl;
	extra->ds  = am.ds;
	extra->st  = am.st;
	extra->rs  = am.rs;
	extra->p_fall = am.p_fall;
	extra->fq  = am.fq; /* Required to fix toneporta */

#if 0
	/*
	 * Implement P.FALL using pitch envelope
	 */

	if (am.p_fall) {
		freq_env->npt = 2;
		freq_env->flg = XMP_ENVELOPE_ON;
		freq_env->data[0] = 0;
		freq_env->data[1] = 0;
		freq_env->data[2] = 1024 / abs(am.p_fall);
		freq_env->data[3] = 10 * (am.p_fall > 0 ? -256 : 256);
	}
#endif

	if (libxmp_load_sample(m, NULL, SAMPLE_FLAG_NOLOAD, xxs, wave))
		return -1;

	return 0;
}

static HIO_HANDLE *flt_check_sample_file(struct libxmp_path *sp,
					 size_t ext_pos, const char *ext)
{
	if (libxmp_path_suffix_at(sp, ext_pos, ext) != 0)
		return NULL;

	return hio_open(sp->path, "rb");
}

static int flt_load(struct module_data *m, HIO_HANDLE * f, const int start)
{
	struct xmp_module *mod = &m->mod;
	int i, j;
	struct xmp_event *event;
	struct mod_header mh;
	uint8 mod_event[4];
	const char *tracker;
	struct libxmp_path sp;
	size_t sp_length;
	char buf[16];
	HIO_HANDLE *nt = NULL;
	int am_synth;

	LOAD_INIT();

	/* See if we have the synth parameters file */
	am_synth = 0;
	libxmp_path_init(&sp);
	if (libxmp_path_join(&sp, m->dirname, m->basename) == 0) {
		sp_length = sp.length;

		nt = flt_check_sample_file(&sp, sp_length, ".NT");
		if (nt == NULL) {
			nt = flt_check_sample_file(&sp, sp_length, ".nt");
		}
		if (nt == NULL) {
			nt = flt_check_sample_file(&sp, sp_length, ".AS");
		}
		if (nt == NULL) {
			nt = flt_check_sample_file(&sp, sp_length, ".as");
		}
	}
	libxmp_path_free(&sp);

	tracker = "Startrekker";

	if (nt) {
		if (hio_read(buf, 1, 16, nt) != 16) {
			goto err;
		}
		if (memcmp(buf, "ST1.2 ModuleINFO", 16) == 0) {
			am_synth = 1;
			tracker = "Startrekker 1.2";
		} else if (memcmp(buf, "ST1.3 ModuleINFO", 16) == 0) {
			am_synth = 1;
			tracker = "Startrekker 1.3";
		} else if (memcmp(buf, "AudioSculpture10", 16) == 0) {
			am_synth = 1;
			tracker = "AudioSculpture 1.0";
		}
	}

	hio_read(mh.name, 20, 1, f);
	for (i = 0; i < 31; i++) {
		hio_read(mh.ins[i].name, 22, 1, f);
		mh.ins[i].size = hio_read16b(f);
		mh.ins[i].finetune = hio_read8(f);
		mh.ins[i].volume = hio_read8(f);
		mh.ins[i].loop_start = hio_read16b(f);
		mh.ins[i].loop_size = hio_read16b(f);
	}
	mh.len = hio_read8(f);
	mh.restart = hio_read8(f);
	hio_read(mh.order, 128, 1, f);
	hio_read(mh.magic, 4, 1, f);

	if (mh.magic[3] == '4') {
		mod->chn = 4;
	} else {
		mod->chn = 8;
	}

	mod->ins = 31;
	mod->smp = mod->ins;
	mod->len = mh.len;
	mod->rst = mh.restart;
	memcpy(mod->xxo, mh.order, 128);

	for (i = 0; i < 128; i++) {
		if (mod->chn > 4)
			mod->xxo[i] >>= 1;
		if (mod->xxo[i] > mod->pat)
			mod->pat = mod->xxo[i];
	}

	mod->pat++;

	mod->trk = mod->chn * mod->pat;

	if (am_synth && libxmp_flt_new_module_extras(m) < 0)
		return -1;

	strncpy(mod->name, (char *)mh.name, 20);
	libxmp_set_type(m, "%s %4.4s", tracker, mh.magic);
	MODULE_INFO();

	if (libxmp_init_instrument(m) < 0)
		goto err;

	for (i = 0; i < mod->ins; i++) {
		struct xmp_instrument *xxi = &mod->xxi[i];
		struct xmp_sample *xxs = &mod->xxs[i];
		struct xmp_subinstrument *sub;

		if (libxmp_alloc_subinstrument(mod, i, 1) < 0)
			goto err;

		sub = &xxi->sub[0];

		xxs->len = 2 * mh.ins[i].size;
		xxs->lps = 2 * mh.ins[i].loop_start;
		xxs->lpe = xxs->lps + 2 * mh.ins[i].loop_size;
		xxs->flg = mh.ins[i].loop_size > 1 ? XMP_SAMPLE_LOOP : 0;
		sub->fin = (int8) ((uint8)mh.ins[i].finetune << 4);
		sub->vol = mh.ins[i].volume;
		sub->pan = XMP_INST_NO_DEFAULT_PAN;
		sub->sid = i;
		xxi->rls = 0xfff;

		if (xxs->len > 0)
			xxi->nsm = 1;

		libxmp_instrument_name(mod, i, mh.ins[i].name, 22);
	}

	if (libxmp_init_pattern(mod) < 0)
		goto err;

	/* Load and convert patterns */
	D_(D_INFO "Stored patterns: %d", mod->pat);

	/* "The format you are looking for is FLT8, and the ONLY two
	 *  differences are: It says FLT8 instead of FLT4 or M.K., AND, the
	 *  patterns are PAIRED. I thought this was the easiest 8 track
	 *  format possible, since it can be loaded in a normal 4 channel
	 *  tracker if you should want to rip sounds or patterns. So, in a
	 *  8 track FLT8 module, patterns 00 and 01 is "really" pattern 00.
	 *  Patterns 02 and 03 together is "really" pattern 01. That's it.
	 *  Oh well, I didn't have the time to implement all effect commands
	 *  either, so some FLT8 modules would play back badly (I think
	 *  especially the portamento command uses a different "scale" than
	 *  the normal portamento command, that would be hard to patch).
	 */
	for (i = 0; i < mod->pat; i++) {
		if (libxmp_alloc_pattern_tracks(mod, i, 64) < 0)
			goto err;

		for (j = 0; j < (64 * 4); j++) {
			event = &EVENT(i, j % 4, j / 4);
			if (hio_read(mod_event, 1, 4, f) < 4) {
				D_(D_CRIT "read error at pat %d", i);
				goto err;
			}
			libxmp_decode_noisetracker_event(event, mod_event);
		}
		if (mod->chn > 4) {
			for (j = 0; j < (64 * 4); j++) {
				event = &EVENT(i, (j % 4) + 4, j / 4);
				if (hio_read(mod_event, 1, 4, f) < 4) {
					D_(D_CRIT "read error at pat %d", i);
					goto err;
				}
				libxmp_decode_noisetracker_event(event, mod_event);

				/* no macros */
				if (event->fxt == 0x0e)
					event->fxt = event->fxp = 0;
			}
		}
	}

	/* no such limit for synth instruments
	 * mod->flg |= XXM_FLG_MODRNG;
	 */

	/* Load samples */

	D_(D_INFO "Stored samples: %d", mod->smp);

	for (i = 0; i < mod->smp; i++) {
		if (am_synth && is_am_instrument(nt, i)) {
			if (hio_seek(f, mod->xxs[i].len, SEEK_CUR) < 0) {
				D_(D_CRIT "seek error at AM instrument %d", i);
				goto err;
			}
			if (read_am_instrument(m, nt, i) < 0) {
				D_(D_CRIT "error loading AM instrument %d", i);
				goto err;
			}
			continue;
		}
		if (libxmp_load_sample(m, f, SAMPLE_FLAG_FULLREP, &mod->xxs[i], NULL) <
		    0) {
			goto err;
		}
	}

	if (nt) {
		hio_close(nt);
	}

	return 0;

      err:
	if (nt) {
		hio_close(nt);
	}

	return -1;
}
