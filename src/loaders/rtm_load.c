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

#include "loader.h"
#include "../period.h"

/* Data structures from the specification of the RTM format version 1.10 by
 * Arnaud Hasenfratz
 */

struct ObjectHeader {
	char id[4];		/* "RTMM", "RTND", "RTIN" or "RTSM" */
	char rc;		/* 0x20 */
	char name[32];		/* object name */
	char eof;		/* "\x1A" */
	uint16 version;		/* version of the format (actual : 0x110) */
	uint16 headerSize;	/* object header size */
};

struct RTMMHeader {		/* Real Tracker Music Module */
	char software[20];	/* software used for saving the module */
	char composer[32];
#define RTMM_FLAG_LINEAR_TABLE	(1 << 0)
#define RTMM_FLAG_TRACK_NAMES	(1 << 1)
	uint16 flags;		/* song flags */
				/* bit 0 : linear table,
				   bit 1 : track names present */
	uint8 ntrack;		/* number of tracks */
	uint8 ninstr;		/* number of instruments */
	uint16 nposition;	/* number of positions */
	uint16 npattern;	/* number of patterns */
	uint8 speed;		/* initial speed */
	uint8 tempo;		/* initial tempo */
	int8 panning[32];	/* initial pannings (for S3M compatibility) */
	uint32 extraDataSize;	/* length of data after the header */

/* version 1.12 */
	char originalName[32];
};

struct RTNDHeader {		/* Real Tracker Note Data */
	uint16 flags;		/* Always 1 */
	uint8 ntrack;
	uint16 nrows;
	uint32 datasize;	/* Size of packed data */
};

struct EnvelopePoint {
	int32 x;
	int32 y;
};

struct Envelope {
	uint8 npoint;
	struct EnvelopePoint point[12];
	uint8 sustain;
	uint8 loopstart;
	uint8 loopend;
#define RTENV_FLAG_ENABLE	(1 << 0)
#define RTENV_FLAG_SUSTAIN	(1 << 1)
#define RTENV_FLAG_LOOP		(1 << 2)
	uint16 flags;		/* bit 0 : enable envelope,
				   bit 1 : sustain, bit 2 : loop */
};

struct RTINHeader {		/* Real Tracker Instrument */
	uint8 nsample;
#define RTIN_FLAG_DEFAULT_PAN	(1 << 0)
#define RTIN_FLAG_MUTE_SAMPLES	(1 << 1)
	uint16 flags;		/* bit 0 : default panning enabled
				   bit 1 : mute samples */
	uint8 table[120];	/* sample number for each note */
	struct Envelope volumeEnv;
	struct Envelope panningEnv;
	int8 vibflg;		/* vibrato type */
	int8 vibsweep;		/* vibrato sweep */
	int8 vibdepth;		/* vibrato depth */
	int8 vibrate;		/* vibrato rate */
	uint16 volfade;

/* version 1.10 */
	uint8 midiPort;
	uint8 midiChannel;
	uint8 midiProgram;
	uint8 midiEnable;

/* version 1.12 */
	int8 midiTranspose;
	uint8 midiBenderRange;
	uint8 midiBaseVolume;
	int8 midiUseVelocity;
};

struct RTSMHeader {		/* Real Tracker Sample */
#define RTSM_FLAG_16_BIT	(1 << 1)
#define RTSM_FLAG_DELTA_CODING	(1 << 2)
	uint16 flags;		/* bit 1 : 16 bits,
				   bit 2 : delta encoded (always) */
	uint8 basevolume;
	uint8 defaultvolume;
	uint32 length;
#define RTSM_LOOP_ON		1
#define RTSM_LOOP_BIDIR		2
	uint8 loop;		/* =0:no loop, =1:forward loop,
				   =2:bi-directional loop */
	uint8 reserved[3];
	uint32 loopbegin;
	uint32 loopend;
	uint32 basefreq;
	uint8 basenote;
	int8 panning;		/* Panning from -64 to 64 */
};


static int rtm_test(HIO_HANDLE *, char *, const int);
static int rtm_load (struct module_data *, HIO_HANDLE *, const int);

const struct format_loader libxmp_loader_rtm = {
	"Real Tracker",
	rtm_test,
	rtm_load
};

static int rtm_test(HIO_HANDLE *f, char *t, const int start)
{
	char buf[4];

	if (hio_read(buf, 1, 4, f) < 4)
		return -1;
	if (memcmp(buf, "RTMM", 4))
		return -1;

	if (hio_read8(f) != 0x20)
		return -1;

	libxmp_read_title(f, t, 32);

	return 0;
}


#define MAX_SAMP 1024

#define FX_NONE			0xff
#define FX_EXTENDED_IT		0xfe
#define FX_PORTA_UP_MOD		0xf1
#define FX_PORTA_DN_MOD		0xf2
#define FX_TONE_VSLIDE_MOD	0xf5
#define FX_VIBRA_VSLIDE_MOD	0xf6
#define FX_VOLSLIDE_MOD		0xfa

/* MOD effects and S3M effects have separate effects memory.
 *
 * TODO: Axy/5xy/6xy have shared memory and dxy/kxy have shared memory.
 * Additionally, dxy will set the Axy memory if the dxy slide is NOT fine,
 * but Axy will never set the memory of dxy. Effects memory is positional,
 * i.e. effect 1 and effect 2 of a given channel have their own memory slots.
 * Simulating this would be a lot of work for a format that was barely used,
 * so just strip "fine" effects from Axy/5xy/6xy and let the two sets share
 * memory for now.
 *
 * TODO: 1xx has separate memory from fxx; 2xx has separate memory from exx.
 */
static const uint8 rtm_fx[41] = {
	/* 0 */ FX_ARPEGGIO,		/* Arpeggio (MOD) */
	/* 1 */ FX_PORTA_UP_MOD,	/* Portamento up (MOD) */
	/* 2 */ FX_PORTA_DN_MOD,	/* Portamento down (MOD) */
	/* 3 */ FX_TONEPORTA,		/* Toneporta */
	/* 4 */ FX_VIBRATO,		/* Vibrato */
	/* 5 */ FX_TONE_VSLIDE_MOD,	/* Toneporta + volume slide (MOD) */
	/* 6 */ FX_VIBRA_VSLIDE_MOD,	/* Vibrato + volume slide (MOD) */
	/* 7 */ FX_TREMOLO,		/* Tremolo */
	/* 8 */ FX_SETPAN,		/* Set panning (S3M) */
	/* 9 */ FX_OFFSET,		/* Sample offset */
	/* A */ FX_VOLSLIDE_MOD,	/* Volume slide (MOD) */
	/* B */ FX_JUMP,		/* Position jump */
	/* C */ FX_VOLSET,		/* Set volume */
	/* D */ FX_BREAK,		/* Pattern break */
	/* E */ FX_EXTENDED,		/* Extended effects (MOD) */
	/* F */ FX_SPEED,		/* Set speed/tempo */
	/* G */ FX_GLOBALVOL,		/* Set global volume */
	/* H */ FX_GVOL_SLIDE,		/* Global volume slide */
	/* I */ FX_NONE,
	/* J */ FX_NONE,
	/* K */ FX_KEYOFF,		/* Key off */
	/* L */ FX_ENVPOS,		/* Set volume envelope position */
	/* M */ FX_NONE,		/* Select MIDI controller */
	/* N */ FX_NONE,
	/* O */ FX_NONE,
	/* P */ FX_PANSLIDE,		/* Panning slide */
	/* Q */ FX_NONE,
	/* R */ FX_MULTI_RETRIG,	/* Retrig + volume slide */
	/* S */ FX_EXTENDED_IT,		/* SA Set high sample offset */
	/* T */ FX_TREMOR,		/* Tremor */
	/* U */ FX_NONE,
	/* V */ FX_NONE,		/* Set MIDI controller value */
	/* W */ FX_NONE,
	/* X */ FX_XF_PORTA,		/* Extra fine portamento */
	/* Y */ FX_NONE,
	/* Z */ FX_NONE,
	/* d */ FX_VOLSLIDE,		/* Volume slide (S3M) */
	/* f */ FX_PORTA_UP,		/* Portamento up (S3M) */
	/* e */ FX_PORTA_DN,		/* Portamento down (S3M) */
	/* k */ FX_VIBRA_VSLIDE,	/* Vibrato + volume slide (S3M) */
	/* a */ FX_S3M_SPEED,		/* Set speed (S3M) */
};

static void rtm_translate_effect(uint8 *fxt, uint8 *fxp)
{
	if (*fxt >= ARRAY_SIZE(rtm_fx)) {
		*fxt = *fxp = 0;
		return;
	}

	*fxt = rtm_fx[*fxt];
	switch (*fxt) {
	case FX_NONE:
		*fxt = *fxp = 0;
		break;

	case FX_SETPAN:				/* Set panning (S3M) */
		if (*fxp == 0xa4) {
			*fxt = FX_SURROUND;
			*fxp = 1;
		} else {
			int p = ((int)*fxp) << 1;
			*fxp = MIN(p, 255);
		}
		break;

	case FX_VOLSLIDE_MOD:			/* Volume slide (MOD) */
	case FX_TONE_VSLIDE_MOD:		/* Toneporta + volslide (MOD) */
	case FX_VIBRA_VSLIDE_MOD:		/* Vibrato + volslide (MOD) */
		/* TODO: this format has very strange memory quirks that
		 * are not emulated, see above. */
		*fxt &= 0x0f;
		/* Disable fine effects */
		if (LSN(*fxp) && MSN(*fxp)) {
			*fxp &= 0xf0;
		}
		break;

	case FX_PORTA_UP_MOD:			/* Portamento up (MOD) */
	case FX_PORTA_DN_MOD:			/* Portamento down (MOD) */
		/* TODO: 1xx has separate memory from fxx,
		 * 2xx has separate memory from exx.
		 * Clamp down values that would be interpreted as fine.
		 * Values this high are essentially indistinguishable. */
		*fxt &= 0x0f;
		*fxp = MIN(*fxp, 0xdf);
		break;

	case FX_EXTENDED:			/* Extended effects (MOD) */
		switch MSN(*fxp) {
		case 0x0:			/* Not implemented */
		case 0xf:
			*fxt = *fxp = 0;
			break;
		case EX_VIBRATO_WF:		/* Set vibrato control */
		case EX_TREMOLO_WF:		/* Set tremolo control */
			/* TODO: 0=sine, 1=ramp-up, 2=ramp-down */
			break;
		}
		break;

	case FX_EXTENDED_IT:			/* Extended effects (IT) */
		switch MSN(*fxp) {
		case 0xa:			/* Set high sample offset */
			*fxt = FX_HIOFFSET;
			*fxp = LSN(*fxp);
			break;
		default:			/* Not implemented */
			*fxt = *fxp = 0;
			break;
		}
		break;
	}
}

/* Convert Real Tracker -64..64 pan values into xmp 0..255 pan values. */
static int rtm_convert_pan(int8 pan)
{
	int v = ((int)pan << 1) + 0x80;
	CLAMP(v, 0, 255);
	return v;
}

static int rtm_convert_envelope_flags(uint16 flags)
{
	/* RTM envelope flags are XM-compatible */
	return flags & (RTENV_FLAG_ENABLE | RTENV_FLAG_SUSTAIN | RTENV_FLAG_LOOP);
}


static int read_object_header(HIO_HANDLE *f, struct ObjectHeader *h, const char *id)
{
	hio_read(h->id, 4, 1, f);
	D_(D_WARN "object id: %02x %02x %02x %02x", h->id[0],
					h->id[1], h->id[2], h->id[3]);

	if (memcmp(id, h->id, 4))
		return -1;

	h->rc = hio_read8(f);
	if (h->rc != 0x20)
		return -1;
	if (hio_read(h->name, 1, 32, f) != 32)
		return -1;
	h->eof = hio_read8(f);
	h->version = hio_read16l(f);
	h->headerSize = hio_read16l(f);
	D_(D_INFO "object %-4.4s (%d)", h->id, h->headerSize);

	return 0;
}


static int rtm_load(struct module_data *m, HIO_HANDLE *f, const int start)
{
	struct xmp_module *mod = &m->mod;
	int i, j, r;
	struct xmp_event *event;
	struct ObjectHeader oh;
	struct RTMMHeader rh;
	struct RTNDHeader rp;
	struct RTINHeader ri;
	struct RTSMHeader rs;
	int offset, smpnum, version;
	char tracker_name[21], composer[33];

	LOAD_INIT();

	if (read_object_header(f, &oh, "RTMM") < 0)
		return -1;

	version = oh.version;

	hio_read(tracker_name, 1, 20, f);
	tracker_name[20] = 0;
	hio_read(composer, 1, 32, f);
	composer[32] = 0;
	rh.flags = hio_read16l(f);
	rh.ntrack = hio_read8(f);
	rh.ninstr = hio_read8(f);
	rh.nposition = hio_read16l(f);
	rh.npattern = hio_read16l(f);
	rh.speed = hio_read8(f);
	rh.tempo = hio_read8(f);
	hio_read(rh.panning, 32, 1, f);
	rh.extraDataSize = hio_read32l(f);

	/* Sanity check */
	if (hio_error(f) || rh.nposition > 255 || rh.ntrack > 32 || rh.npattern > 255) {
		return -1;
	}

	if (version >= 0x0112)
		hio_seek(f, 32, SEEK_CUR);		/* skip original name */

	for (i = 0; i < rh.nposition; i++) {
		mod->xxo[i] = hio_read16l(f);
		if (mod->xxo[i] >= rh.npattern) {
			return -1;
		}
	}

	strncpy(mod->name, oh.name, 32);
	mod->name[32] = '\0';
	snprintf(mod->type, XMP_NAME_SIZE, "%s RTM %x.%02x",
				tracker_name, version >> 8, version & 0xff);
	/* strncpy(m->author, composer, XMP_NAME_SIZE); */

	mod->len = rh.nposition;
	mod->pat = rh.npattern;
	mod->chn = rh.ntrack;
	mod->trk = mod->chn * mod->pat;
	mod->ins = rh.ninstr;
	mod->spd = rh.speed;
	mod->bpm = rh.tempo;

	m->c4rate = C4_NTSC_RATE;
	m->period_type = rh.flags & RTMM_FLAG_LINEAR_TABLE ? PERIOD_LINEAR : PERIOD_AMIGA;

	MODULE_INFO();

	for (i = 0; i < mod->chn; i++)
		mod->xxc[i].pan = rtm_convert_pan(rh.panning[i]);

	if (libxmp_init_pattern(mod) < 0)
		return -1;

	D_(D_INFO "Stored patterns: %d", mod->pat);

	offset = 42 + oh.headerSize + rh.extraDataSize;

	for (i = 0; i < mod->pat; i++) {
		uint8 c;

		hio_seek(f, start + offset, SEEK_SET);

		if (read_object_header(f, &oh, "RTND") < 0) {
			D_(D_CRIT "Error reading pattern %d", i);
			return -1;
		}

		rp.flags = hio_read16l(f);
		rp.ntrack = hio_read8(f);
		rp.nrows = hio_read16l(f);
		rp.datasize = hio_read32l(f);

		/* Sanity check */
		if (rp.ntrack > rh.ntrack || rp.nrows > 999) {
			return -1;
		}

		offset += 42 + oh.headerSize + rp.datasize;

		if (libxmp_alloc_pattern_tracks_long(mod, i, rp.nrows) < 0)
			return -1;

		for (r = 0; r < rp.nrows; r++) {
			for (j = 0; /*j < rp.ntrack */; j++) {

				c = hio_read8(f);
				if (c == 0)		/* next row */
					break;

				/* Sanity check */
				if (j >= rp.ntrack) {
					return -1;
				}

				event = &EVENT(i, j, r);

				if (c & 0x01) {		/* set track */
					j = hio_read8(f);

					/* Sanity check */
					if (j >= rp.ntrack) {
						return -1;
					}

					event = &EVENT(i, j, r);
				}
				if (c & 0x02) {		/* read note */
					event->note = hio_read8(f) + 1;
					if (event->note == 0xff) {
						event->note = XMP_KEY_OFF;
					} else {
						event->note += 12;
					}
				}
				if (c & 0x04)		/* read instrument */
					event->ins = hio_read8(f);
				if (c & 0x08)		/* read effect */
					event->fxt = hio_read8(f);
				if (c & 0x10)		/* read parameter */
					event->fxp = hio_read8(f);
				if (c & 0x20)		/* read effect 2 */
					event->f2t = hio_read8(f);
				if (c & 0x40)		/* read parameter 2 */
					event->f2p = hio_read8(f);

				if (c & 0x18)
					rtm_translate_effect(&event->fxt, &event->fxp);
				if (c & 0x60)
					rtm_translate_effect(&event->f2t, &event->f2p);
			}
		}
	}

	/*
	 * load instruments
	 */

	D_(D_INFO "Instruments: %d", mod->ins);

	hio_seek(f, start + offset, SEEK_SET);

	/* ESTIMATED value! We don't know the actual value at this point */
	mod->smp = MAX_SAMP;

	if (libxmp_init_instrument(m) < 0)
		return -1;

	smpnum = 0;
	for (i = 0; i < mod->ins; i++) {
		struct xmp_instrument *xxi = &mod->xxi[i];

		if (read_object_header(f, &oh, "RTIN") < 0) {
			D_(D_CRIT "Error reading instrument %d", i);
			return -1;
		}

		libxmp_instrument_name(mod, i, (uint8 *)oh.name, 32);

		if (oh.headerSize == 0) {
			D_(D_INFO "[%2X] %-32.32s %2d ", i,
						xxi->name, xxi->nsm);
			ri.nsample = 0;
			continue;
		}

		ri.nsample = hio_read8(f);
		ri.flags = hio_read16l(f);
		if (hio_read(ri.table, 1, 120, f) != 120)
			return -1;

		ri.volumeEnv.npoint = hio_read8(f);

		/* Sanity check */
		if (ri.volumeEnv.npoint >= 12)
			return -1;

		for (j = 0; j < 12; j++) {
			ri.volumeEnv.point[j].x = hio_read32l(f);
			ri.volumeEnv.point[j].y = hio_read32l(f);
		}
		ri.volumeEnv.sustain = hio_read8(f);
		ri.volumeEnv.loopstart = hio_read8(f);
		ri.volumeEnv.loopend = hio_read8(f);
		ri.volumeEnv.flags = hio_read16l(f);

		ri.panningEnv.npoint = hio_read8(f);

		/* Sanity check */
		if (ri.panningEnv.npoint >= 12)
			return -1;

		for (j = 0; j < 12; j++) {
			ri.panningEnv.point[j].x = hio_read32l(f);
			ri.panningEnv.point[j].y = hio_read32l(f);
		}
		ri.panningEnv.sustain = hio_read8(f);
		ri.panningEnv.loopstart = hio_read8(f);
		ri.panningEnv.loopend = hio_read8(f);
		ri.panningEnv.flags = hio_read16l(f);

		ri.vibflg = hio_read8(f);
		ri.vibsweep = hio_read8(f);
		ri.vibdepth = hio_read8(f);
		ri.vibrate = hio_read8(f);
		ri.volfade = hio_read16l(f);

		if (version >= 0x0110) {
			ri.midiPort = hio_read8(f);
			ri.midiChannel = hio_read8(f);
			ri.midiProgram = hio_read8(f);
			ri.midiEnable = hio_read8(f);
		}
		if (version >= 0x0112) {
			ri.midiTranspose = hio_read8(f);
			ri.midiBenderRange = hio_read8(f);
			ri.midiBaseVolume = hio_read8(f);
			ri.midiUseVelocity = hio_read8(f);
		}

		xxi->nsm = ri.nsample;
		xxi->vol = (~ri.flags & RTIN_FLAG_MUTE_SAMPLES) ? m->volbase : 0;

		D_(D_INFO "[%2X] %-32.32s %2d", i, xxi->name, xxi->nsm);

		if (xxi->nsm > 16)
			xxi->nsm = 16;

		if (libxmp_alloc_subinstrument(mod, i, xxi->nsm) < 0)
			return -1;

		for (j = 0; j < 120; j++)
			xxi->map[j].ins = ri.table[j];

		/* Envelope */
		xxi->rls = ri.volfade;
		xxi->aei.npt = ri.volumeEnv.npoint;
		xxi->aei.sus = ri.volumeEnv.sustain;
		xxi->aei.lps = ri.volumeEnv.loopstart;
		xxi->aei.lpe = ri.volumeEnv.loopend;
		xxi->aei.flg = rtm_convert_envelope_flags(ri.volumeEnv.flags);
		xxi->pei.npt = ri.panningEnv.npoint;
		xxi->pei.sus = ri.panningEnv.sustain;
		xxi->pei.lps = ri.panningEnv.loopstart;
		xxi->pei.lpe = ri.panningEnv.loopend;
		xxi->pei.flg = rtm_convert_envelope_flags(ri.panningEnv.flags);

		for (j = 0; j < xxi->aei.npt; j++) {
			xxi->aei.data[j * 2 + 0] = ri.volumeEnv.point[j].x;
			xxi->aei.data[j * 2 + 1] = ri.volumeEnv.point[j].y / 2;
		}
		for (j = 0; j < xxi->pei.npt; j++) {
			xxi->pei.data[j * 2 + 0] = ri.panningEnv.point[j].x;
			xxi->pei.data[j * 2 + 1] = 32 + ri.panningEnv.point[j].y / 2;
		}

		/* For each sample */
		for (j = 0; j < xxi->nsm; j++, smpnum++) {
			struct xmp_subinstrument *sub = &xxi->sub[j];
			struct xmp_sample *xxs;
			int flags = 0;

			if (read_object_header(f, &oh, "RTSM") < 0) {
				D_(D_CRIT "Error reading sample %d", j);
				return -1;
			}

			rs.flags = hio_read16l(f);
			rs.basevolume = hio_read8(f);
			rs.defaultvolume = hio_read8(f);
			rs.length = hio_read32l(f);
			rs.loop = hio_read32l(f);
			rs.loopbegin = hio_read32l(f);
			rs.loopend = hio_read32l(f);
			rs.basefreq = hio_read32l(f);
			rs.basenote = hio_read8(f);
			rs.panning = hio_read8(f);

			libxmp_c2spd_to_note(rs.basefreq, &sub->xpo, &sub->fin);
			sub->xpo += 48 - rs.basenote;
			sub->vol = rs.defaultvolume;
			sub->gvl = rs.basevolume;
			sub->pan = (ri.flags & RTIN_FLAG_DEFAULT_PAN) ?
				rtm_convert_pan(rs.panning) : -1;
			/* Autovibrato oddities:
			 * Wave:  TODO: 0 sine, 1 square, 2 ramp down, 3 ramp up
			 *        All invalid values are also ramp up.
			 * Depth: the UI limits it 0-15, but higher values
			 *        work. Negatives are very broken.
			 * Rate:  the UI limits 0-63; but higher and negative
			 *        values actually work how you would expect!
			 *        Rate is half as fast as libxmp currently.
			 * Sweep: the UI limits 0-255 but loads as signed.
			 *        During playback, it is treated as unsigned.
			 */
			sub->vwf = MIN((uint8)ri.vibflg, 3);
			sub->vde = MAX(ri.vibdepth, 0) << 2;
			sub->vra = (ri.vibrate + (ri.vibrate > 0)) >> 1;
			sub->vsw = (uint8)ri.vibsweep;
			sub->sid = smpnum;

			if (smpnum >= mod->smp) {
				if (libxmp_realloc_samples(m, mod->smp * 3 / 2) < 0)
					return -1;
			}
 			xxs = &mod->xxs[smpnum];

			libxmp_copy_adjust(xxs->name, (uint8 *)oh.name, 31);

			xxs->len = rs.length;
			xxs->lps = rs.loopbegin;
			xxs->lpe = rs.loopend;

			if (rs.flags & RTSM_FLAG_16_BIT) {
				xxs->flg |= XMP_SAMPLE_16BIT;
				xxs->len >>= 1;
				xxs->lps >>= 1;
				xxs->lpe >>= 1;
			}
			if (rs.flags & RTSM_FLAG_DELTA_CODING) {
				flags |= SAMPLE_FLAG_DIFF;
			}
			/* Any non-zero enables looping */
			if (rs.loop >= RTSM_LOOP_ON) {
				xxs->flg |= XMP_SAMPLE_LOOP;
			}
			/* Only RTSM_LOOP_BIDIR enables; only works for GUS? */
			if (rs.loop == RTSM_LOOP_BIDIR) {
				xxs->flg |= XMP_SAMPLE_LOOP_BIDIR;
			}

			D_(D_INFO "  [%1x] %-32.32s", j, xxs->name);
			D_(D_INFO "      %05x%c%05x %05x %c "
					"BV%02x V%02x F%+04d P%02x R%+03d",
				xxs->len,
				xxs->flg & XMP_SAMPLE_16BIT ? '+' : ' ',
				xxs->lps, xxs->lpe,
				xxs->flg & XMP_SAMPLE_LOOP_BIDIR ? 'B' :
				xxs->flg & XMP_SAMPLE_LOOP ? 'L' : ' ',
				sub->gvl, sub->vol, sub->fin, sub->pan, sub->xpo);

			if (libxmp_load_sample(m, f, flags, xxs, NULL) < 0)
				return -1;
		}
	}

	/* Final sample number adjustment */
	if (libxmp_realloc_samples(m, smpnum) < 0)
		return -1;

	m->quirk |= QUIRK_FINEFX | QUIRK_INSVOL;
	m->read_event_type = READ_EVENT_FT2;

	return 0;
}
