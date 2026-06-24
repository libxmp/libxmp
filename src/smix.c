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
#include "period.h"
#include "player.h"
#include "mixer.h"
#include "virtual.h"
#include "hio.h"
#include "loaders/loader.h"


#define WAV_MAGIC_RIFF	MAGIC4('R','I','F','F')
#define WAV_MAGIC_WAVE	MAGIC4('W','A','V','E')
#define WAV_MAGIC_FMT	MAGIC4('f','m','t',' ')
#define WAV_MAGIC_DATA	MAGIC4('d','a','t','a')

#define WAV_PAD(sz)	(((sz) + 1) & ~1)

struct wav_fmt_data
{
#define WAV_FMT_TYPE_PCM	1
	uint16 format;
	uint16 channels;
	uint32 sample_rate;
	uint32 bytes_per_second;
	uint16 bytes_per_frame;
	uint16 sample_bits;
};

static int libxmp_load_wav_sample(struct module_data *m, struct xmp_sample *xxs,
				  struct wav_fmt_data *wav, HIO_HANDLE *f)
{
	uint32 magic;
	uint32 len;
	int flags = 0;

	magic = hio_read32b(f);
	if (magic != WAV_MAGIC_RIFF) {
		return -XMP_ERROR_FORMAT;
	}
	/* len = */ hio_read32l(f);
	magic = hio_read32b(f);
	if (magic != WAV_MAGIC_WAVE) {
		return -XMP_ERROR_FORMAT;
	}

	magic = hio_read32b(f);
	if (magic != WAV_MAGIC_FMT) {
		return -XMP_ERROR_FORMAT;
	}
	len = hio_read32l(f);
	if (len < 16 || len > 0xffff) {
		return -XMP_ERROR_FORMAT;
	}
	wav->format           = hio_read16l(f);
	wav->channels         = hio_read16l(f);
	wav->sample_rate      = hio_read32l(f);
	wav->bytes_per_second = hio_read32l(f);
	wav->bytes_per_frame  = hio_read16l(f);
	wav->sample_bits      = hio_read16l(f);
	if (hio_error(f)) {
		return -XMP_ERROR_FORMAT;
	}

	/* Only PCM, 8-bit/16-bit, 1 or 2 channels supported */
	if (wav->format != WAV_FMT_TYPE_PCM) {
		return -XMP_ERROR_FORMAT;
	}
	if (wav->channels != 1 && wav->channels != 2) {
		return -XMP_ERROR_FORMAT;
	}
	if (wav->sample_bits != 8 && wav->sample_bits != 16) {
		return -XMP_ERROR_FORMAT;
	}
	if (wav->sample_rate < 16 || wav->sample_rate > (uint32)INT_MAX) {
		return -XMP_ERROR_FORMAT;
	}

	/* Sanity check on rate fields */
	if (((wav->sample_bits + 7) >> 3) * wav->channels != wav->bytes_per_frame) {
		return -XMP_ERROR_FORMAT;
	}
	if (wav->bytes_per_second / wav->sample_rate != wav->bytes_per_frame) {
		return -XMP_ERROR_FORMAT;
	}

	/* Skip remaining fmt chunk */
	if (len > 16 && hio_seek(f, WAV_PAD(len - 16), SEEK_CUR) < 0) {
		return -XMP_ERROR_SYSTEM;
	}

	/* Seek to data chunk */
	for (;;) {
		magic = hio_read32b(f);
		len = hio_read32l(f);
		if (hio_error(f)) {
			return -XMP_ERROR_FORMAT;
		}

		if (magic == WAV_MAGIC_DATA) {
			break;
		}

#if LONG_MAX <= 2147483647L
		if (len > (uint32)LONG_MAX) {
			return -XMP_ERROR_SYSTEM;
		}
#endif
		if (hio_seek(f, WAV_PAD((long)len), SEEK_CUR) < 0) {
			return -XMP_ERROR_SYSTEM;
		}
	}
	/* Now positioned at the start of raw data. */

	xxs->len = len / wav->bytes_per_frame;
	xxs->lps = 0;
	xxs->lpe = 0;
	xxs->flg = 0;

	if (wav->sample_bits == 16) {
		xxs->flg |= XMP_SAMPLE_16BIT;
	} else {
		flags |= SAMPLE_FLAG_UNS;
	}

	if (wav->channels == 2) {
		xxs->flg |= XMP_SAMPLE_STEREO;
		flags |= SAMPLE_FLAG_INTERLEAVED;
	}

	if (libxmp_load_sample(m, f, flags, xxs, NULL) < 0) {
		return -XMP_ERROR_SYSTEM;
	}
	return 0;
}

struct xmp_instrument *libxmp_get_instrument(struct context_data *ctx, int ins)
{
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_instrument *xxi;

	if (ins < 0) {
		xxi = NULL;
	} else if (ins < mod->ins) {
		xxi = &mod->xxi[ins];
	} else if (ins < mod->ins + smix->ins) {
		xxi = &smix->xxi[ins - mod->ins];
	} else {
		xxi = NULL;
	}

	return xxi;
}

struct xmp_sample *libxmp_get_sample(struct context_data *ctx, int smp)
{
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_sample *xxs;

	if (smp < 0) {
		xxs = NULL;
	} else if (smp < mod->smp) {
		xxs = &mod->xxs[smp];
	} else if (smp < mod->smp + smix->smp) {
		xxs = &smix->xxs[smp - mod->smp];
	} else {
		xxs = NULL;
	}

	return xxs;
}

int xmp_start_smix(xmp_context opaque, int chn, int smp)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct smix_data *smix = &ctx->smix;
	int smp_alloc;

	if (ctx->state > XMP_STATE_LOADED) {
		return -XMP_ERROR_STATE;
	}

	if (chn < 1 || smp < 0) {
		return -XMP_ERROR_INVALID;
	}
	/* May already be initialized. Previously, this call
	 * overwrote the old smix state and leaked it.  */
	xmp_end_smix(opaque);

	/* Sample count of 0 previously had undefined behavior,
	 * possibly failing to allocate zero instruments/samples.
	 * Allow it to allocate on the chance anything used it,
	 * but do not allow usage of the extra sample. */
	smp_alloc = MAX(smp, 1);

	smix->xxi = (struct xmp_instrument *) calloc(smp_alloc, sizeof(struct xmp_instrument));
	if (smix->xxi == NULL) {
		goto err;
	}
	smix->xxs = (struct xmp_sample *) calloc(smp_alloc, sizeof(struct xmp_sample));
	if (smix->xxs == NULL) {
		goto err1;
	}

	smix->chn = chn;
	smix->ins = smix->smp = smp;

	return 0;

    err1:
	free(smix->xxi);
	smix->xxi = NULL;
    err:
	return -XMP_ERROR_INTERNAL;
}

int xmp_smix_play_instrument(xmp_context opaque, int ins, int note, int vol, int chn)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct player_data *p = &ctx->p;
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_event *event;

	if (ctx->state < XMP_STATE_PLAYING) {
		return -XMP_ERROR_STATE;
	}

	if (chn >= smix->chn || chn < 0 || ins >= mod->ins || ins < 0) {
		return -XMP_ERROR_INVALID;
	}

	if (note == 0) {
		note = 60;		/* middle C note number */
	}

	event = &p->inject_event[mod->chn + chn];
	memset(event, 0, sizeof (struct xmp_event));
	event->note = (note < XMP_MAX_KEYS) ? note + 1 : note;
	event->ins = ins + 1;
	event->vol = vol + 1;
	event->_flag = 1;

	return 0;
}

int xmp_smix_play_sample(xmp_context opaque, int ins, int note, int vol, int chn)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct player_data *p = &ctx->p;
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_module *mod = &m->mod;
	struct xmp_event *event;

	if (ctx->state < XMP_STATE_PLAYING) {
		return -XMP_ERROR_STATE;
	}

	if (chn >= smix->chn || chn < 0 || ins >= smix->ins || ins < 0) {
		return -XMP_ERROR_INVALID;
	}

	if (note == 0) {
		note = 60;		/* middle C note number */
	}

	event = &p->inject_event[mod->chn + chn];
	memset(event, 0, sizeof (struct xmp_event));
	event->note = (note < XMP_MAX_KEYS) ? note + 1 : note;
	event->ins = mod->ins + ins + 1;
	event->vol = vol + 1;
	event->_flag = 1;

	return 0;
}

int xmp_smix_channel_pan(xmp_context opaque, int chn, int pan)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct player_data *p = &ctx->p;
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct channel_data *xc;

	if (ctx->state < XMP_STATE_PLAYING) {
		return -XMP_ERROR_STATE;
	}

	if (chn >= smix->chn || chn < 0 || pan < 0 || pan > 255) {
		return -XMP_ERROR_INVALID;
	}

	xc = &p->xc_data[m->mod.chn + chn];
	xc->pan.val = pan;

	return 0;
}

int xmp_smix_load_sample(xmp_context opaque, int num, const char *path)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct smix_data *smix = &ctx->smix;
	struct module_data *m = &ctx->m;
	struct xmp_instrument *xxi;
	struct xmp_sample *xxs;
	HIO_HANDLE *h;
	struct wav_fmt_data wav;
	int retval;

	if (num >= smix->ins || num < 0) {
		return -XMP_ERROR_INVALID;
	}

	xxi = &smix->xxi[num];
	xxs = &smix->xxs[num];

	h = hio_open(path, "rb");
	if (h == NULL) {
		return -XMP_ERROR_SYSTEM;
	}

	/* Release whatever instrument/sample exists, if any.
	 * Prior versions leaked and potentially left the
	 * instrument/sample in an undefined state. */
	xmp_smix_release_sample(opaque, num);

	/* Init instrument */

	xxi->sub = (struct xmp_subinstrument *) calloc(1, sizeof(struct xmp_subinstrument));
	if (xxi->sub == NULL) {
		hio_close(h);
		return -XMP_ERROR_SYSTEM;
	}

	xxi->vol = m->volbase;
	xxi->nsm = 1;
	xxi->sub[0].sid = num;
	xxi->sub[0].vol = xxi->vol;
	xxi->sub[0].pan = 0x80;

	/* Load sample */

	retval = libxmp_load_wav_sample(m, xxs, &wav, h);
	hio_close(h);
	if (retval != 0) {
		free(xxi->sub);
		xxi->sub = NULL;
		return retval;
	}

	libxmp_c2spd_to_note(wav.sample_rate, &xxi->sub[0].xpo, &xxi->sub[0].fin);

	return 0;
}

int xmp_smix_release_sample(xmp_context opaque, int num)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct smix_data *smix = &ctx->smix;
	struct player_data *p = &ctx->p;
	struct xmp_instrument *xxi;
	struct xmp_sample *xxs;
	int i;

	if (num >= smix->ins || num < 0) {
		return -XMP_ERROR_INVALID;
	}

	xxi = &smix->xxi[num];
	xxs = &smix->xxs[num];

	/* This sample may be actively playing in the mixer. Fully clear
	 * any mixer voice using it to avoid any playback issues/crashes. */
	for (i = 0; i < p->virt.maxvoc; i++) {
		struct mixer_voice *vi = &p->virt.voice_array[i];

		if (vi->sptr == xxs->data) {
			libxmp_virt_resetvoice(ctx, i, 1);
		}
	}

	libxmp_free_sample(xxs);
	free(xxi->sub);

	xxs->data = NULL;
	xxi->sub = NULL;

	return 0;
}

void xmp_end_smix(xmp_context opaque)
{
	struct context_data *ctx = (struct context_data *)opaque;
	struct smix_data *smix = &ctx->smix;
	int i;

	if (smix->xxs == NULL) {
		return;
	}

	for (i = 0; i < smix->smp; i++) {
		xmp_smix_release_sample(opaque, i);
	}

	free(smix->xxs);
	free(smix->xxi);
	smix->xxs = NULL;
	smix->xxi = NULL;
	smix->chn = 0;
	smix->ins = 0;
	smix->smp = 0;
}
