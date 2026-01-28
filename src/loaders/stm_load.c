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
#include "../path.h"
#include "../period.h"

/* STM tempo is calculated based on the mix buffer size, which requires knowing
 * the mix rate of ST2. This is the highest mix rate in released versions. */
#define STM_MIX_RATE	23863

#define STM_TYPE_SONG	0x01
#define STM_TYPE_MODULE	0x02

struct stm_instrument_header {
	uint8 name[13];		/* Instrument name in 8.3 format (ASCIIZ) */
	uint8 idisk;		/* Instrument disk */
	uint16 rsvd1;		/* Reserved */
	uint16 length;		/* Sample length */
	uint16 loopbeg;		/* Loop begin */
	uint16 loopend;		/* Loop end */
	uint8 volume;		/* Playback volume */
	uint8 rsvd2;		/* Reserved */
	uint16 c2spd;		/* C4 speed */
	uint32 rsvd3;		/* Reserved */
	uint16 paralen;		/* Length in paragraphs */
};

/* v1 format header based on disassembled ST2 */
struct stm_file_subheader_v1 {
	uint16 insnum;		/* Number of instruments */
	uint16 ordnum;		/* Number of orders */
	uint16 patnum;		/* Number of patterns */
	uint16 srate;		/* Sample rate? */
	uint8 tempo;		/* Playback tempo */
	uint8 channels;		/* Number of channels */
	uint16 psize;		/* Pattern size */
	uint16 rsvd2;		/* Reserved */
	uint16 skip;		/* Bytes to skip */
};

struct stm_file_subheader_v2 {
	uint8 tempo;		/* Playback tempo */
	uint8 patterns;		/* Number of patterns */
	uint8 gvol;		/* Global volume */
	uint8 rsvd2[13];	/* Reserved */
};

struct stm_file_header {
	uint8 name[20];		/* ASCIIZ song name */
	uint8 magic[8];		/* '!Scream!' */
	uint8 rsvd1;		/* '\x1a' */
	uint8 type;		/* 1=song, 2=module */
	uint8 vermaj;		/* Major version number */
	uint8 vermin;		/* Minor version number */
	union {
		struct stm_file_subheader_v1 v1;
		struct stm_file_subheader_v2 v2;
	} sub;
	struct stm_instrument_header ins[32];
};

static int stm_test(HIO_HANDLE *, char *, const int);
static int stm_load(struct module_data *, HIO_HANDLE *, const int);

const struct format_loader libxmp_loader_stm = {
	"Scream Tracker 2",
	stm_test,
	stm_load
};

static int stm_test(HIO_HANDLE * f, char *t, const int start)
{
	uint8 buf[8];
	uint16 version;

	hio_seek(f, start + 20, SEEK_SET);
	if (hio_read(buf, 1, 8, f) < 8)
		return -1;

	if (libxmp_test_name(buf, 8, 0))	/* Tracker name should be ASCII */
		return -1;

	/* EOF should be 0x1a. putup10.stm and putup11.stm have 2 instead. */
	buf[0] = hio_read8(f);
	if (buf[0] != 0x1a && buf[0] != 0x02)
		return -1;

	if (hio_read8(f) > STM_TYPE_MODULE)
		return -1;

	buf[0] = hio_read8(f);
	buf[1] = hio_read8(f);
	version = (100 * buf[0]) + buf[1];

	if (version != 110 &&
	    version != 200 && version != 210 &&
	    version != 220 && version != 221) {
		D_(D_CRIT "Unknown version: %d", version);
		return -1;
	}

	hio_seek(f, start + 60, SEEK_SET);
	if (hio_read(buf, 1, 4, f) < 4)
		return -1;
	if (!memcmp(buf, "SCRM", 4))	/* We don't want STX files */
		return -1;

	hio_seek(f, start + 0, SEEK_SET);
	libxmp_read_title(f, t, 20);

	return 0;
}

#define FX_NONE		0xff

/*
 * Skaven's note from http://www.futurecrew.com/skaven/oldies_music.html
 *
 * FYI for the tech-heads: In the old Scream Tracker 2 the Arpeggio command
 * (Jxx), if used in a single row with a 0x value, caused the note to skip
 * the specified amount of halftones upwards halfway through the row. I used
 * this in some songs to give the lead some character. However, when played
 * in ModPlug Tracker, this effect doesn't work the way it did back then.
 */
static const uint8 fx[16] = {
	FX_NONE,
	FX_SPEED,	/* A - Set tempo to [INFO]. 60 normal. */
	FX_JUMP,	/* B - Break pattern and jmp to order [INFO] */
	FX_BREAK,	/* C - Break pattern */
	FX_VOLSLIDE,	/* D - Slide volume; Hi-nibble=up, Lo-nibble=down */
	FX_PORTA_DN,	/* E - Slide down at speed [INFO] */
	FX_PORTA_UP,	/* F - Slide up at speed [INFO] */
	FX_TONEPORTA,	/* G - Slide to the note specified at speed [INFO] */
	FX_VIBRATO,	/* H - Vibrato; Hi-nibble, speed. Lo-nibble, size */
	FX_TREMOR,	/* I - Tremor; Hi-nibble, ontime. Lo-nibble, offtime */
	FX_ARPEGGIO,	/* J - Arpeggio */
	FX_NONE,
	FX_NONE,
	FX_NONE,
	FX_NONE,
	FX_NONE
};

/* Following cs127's research fairly closely here out of necessity. Notes:
 * - maximum effective BPM is 125.
 * - divisor may be negative.
 * - the potentially negative quotient of this division may be modified,
 *   thus the mix rate can not be optimized out.
 * - TODO: tempo effects take effect the next tick, not the current tick.
 * - TODO: tempos 1E, 1F, and 05-0F result in BPMs lower than XMP_MIN_BPM.
 */
static int stm_calculate_bpm(int spd, int tempo_factor)
{
	static const uint8 speed_factor[16] = {
		140, 50, 25, 15, 10, 7, 6, 4, 3, 3, 2, 2, 2, 2, 1, 1
	};
	int divisor = 50 - (((int)speed_factor[spd] * tempo_factor) >> 4);
	int tick_frames;

	/* Safety check; 0 should not be possible */
	tick_frames = divisor ? STM_MIX_RATE / divisor : -1;
	tick_frames = (unsigned)tick_frames & 0xffffu;

	/* TODO: BPMs can be non-integer. */
	return (STM_MIX_RATE * 5 + (tick_frames /*round*/)) / (tick_frames * 2);
}

static void stm_convert_tempo(int *spd, int *bpm, unsigned tempo, int version)
{
	if (version >= 221) {
		*spd = MSN(tempo);
		*bpm = stm_calculate_bpm(*spd, LSN(tempo));
	} else if (version >= 110) {
		/* TODO: ST <2.2 behavior unclear. ST 2.2+ imports higher
		 * speeds (effect speed is modulo 16) and then reads the
		 * speed factor table out-of-bounds. */
		*spd = tempo / 10;
		*bpm = stm_calculate_bpm(MIN(*spd, 15), tempo % 10);
	} else {
		*spd = tempo;
		*bpm = 125;
	}
	*spd = MAX(*spd, 1);
	*bpm = MAX(*bpm, XMP_MIN_BPM);
}

static int stm_load(struct module_data *m, HIO_HANDLE * f, const int start)
{
	struct xmp_module *mod = &m->mod;
	struct xmp_event *event;
	struct stm_file_header sfh;
	uint8 b;
	uint16 version;
	int blank_pattern = 0;
	int stored_patterns;
	int spd, bpm;
	int i, j, k;

	LOAD_INIT();

	hio_read(sfh.name, 20, 1, f);	/* ASCIIZ song name */
	hio_read(sfh.magic, 8, 1, f);	/* '!Scream!' */
	sfh.rsvd1 = hio_read8(f);	/* '\x1a' */
	sfh.type = hio_read8(f);	/* 1=song, 2=module */
	sfh.vermaj = hio_read8(f);	/* Major version number */
	sfh.vermin = hio_read8(f);	/* Minor version number */
	version = (100 * sfh.vermaj) + sfh.vermin;

	if (version != 110 &&
	    version != 200 && version != 210 &&
	    version != 220 && version != 221) {
		D_(D_CRIT "Unknown version: %d", version);
		return -1;
	}

	// TODO: improve robustness of the loader against bad parameters

	if (version >= 200) {
		sfh.sub.v2.tempo = hio_read8(f);	/* Playback tempo */
		sfh.sub.v2.patterns = hio_read8(f);	/* Number of patterns */
		sfh.sub.v2.gvol = hio_read8(f);		/* Global volume */
		hio_read(sfh.sub.v2.rsvd2, 13, 1, f);	/* Reserved */
		mod->chn = 4;
		mod->pat = sfh.sub.v2.patterns;

		stm_convert_tempo(&mod->spd, &mod->bpm, sfh.sub.v2.tempo, version);
		mod->ins = 31;
		mod->len = (version == 200) ? 64 : 128;
	} else {
		if ((sfh.sub.v1.insnum = hio_read16l(f)) > 32) {	/* Number of instruments */
			D_(D_CRIT "Wrong number of instruments: %d (max 32)", sfh.sub.v1.insnum);
			return -1;
		}
		if ((sfh.sub.v1.ordnum = hio_read16l(f)) > XMP_MAX_MOD_LENGTH) {	/* Number of orders */
			D_(D_CRIT "Wrong number of orders: %d (max %d)", sfh.sub.v1.ordnum, XMP_MAX_MOD_LENGTH);
			return -1;
		}
		if ((sfh.sub.v1.patnum = hio_read16l(f)) > XMP_MAX_MOD_LENGTH) {	/* Number of patterns */
			D_(D_CRIT "Wrong number of patterns: %d (max %d)", sfh.sub.v1.patnum, XMP_MAX_MOD_LENGTH);
			return -1;
		}
		sfh.sub.v1.srate = hio_read16l(f);	/* Sample rate? */
		sfh.sub.v1.tempo = hio_read8(f);	/* Playback tempo */
		if ((sfh.sub.v1.channels = hio_read8(f)) != 4) {	/* Number of channels */
			D_(D_CRIT "Wrong number of sound channels: %d", sfh.sub.v1.channels);
			return -1;
		}
		if ((sfh.sub.v1.psize = hio_read16l(f)) != 64) {	/* Pattern size */
			D_(D_CRIT "Wrong number of rows per pattern: %d", sfh.sub.v1.psize);
			return -1;
		}
		sfh.sub.v1.rsvd2 = hio_read16l(f);	/* Reserved */
		sfh.sub.v1.skip = hio_read16l(f);	/* Bytes to skip */
		hio_seek(f, sfh.sub.v1.skip, SEEK_CUR);	/* Skip bytes */
		mod->chn = sfh.sub.v1.channels;
		mod->pat = sfh.sub.v1.patnum;

		stm_convert_tempo(&mod->spd, &mod->bpm, sfh.sub.v1.tempo, version);
		mod->ins = sfh.sub.v1.insnum;
		mod->len = sfh.sub.v1.ordnum;
	}

	for (i = 0; i < mod->ins; i++) {
		hio_read(sfh.ins[i].name, 13, 1, f);	/* Instrument name */
		sfh.ins[i].idisk = hio_read8(f);	/* Instrument disk */
		sfh.ins[i].rsvd1 = hio_read16l(f);	/* Reserved */
		sfh.ins[i].length = hio_read16l(f);	/* Sample length */
		sfh.ins[i].loopbeg = hio_read16l(f);	/* Loop begin */
		sfh.ins[i].loopend = hio_read16l(f);	/* Loop end */
		sfh.ins[i].volume = hio_read8(f);	/* Playback volume */
		sfh.ins[i].rsvd2 = hio_read8(f);	/* Reserved */
		sfh.ins[i].c2spd = hio_read16l(f);	/* C4 speed */
		sfh.ins[i].rsvd3 = hio_read32l(f);	/* Reserved */
		sfh.ins[i].paralen = hio_read16l(f);	/* Length in paragraphs */
	}

	if (hio_error(f)) {
		return -1;
	}

	mod->smp = mod->ins;
	m->c4rate = C4_NTSC_RATE;

	libxmp_copy_adjust(mod->name, sfh.name, 20);

	if (!sfh.magic[0] || !strncmp((char *)sfh.magic, "PCSTV", 5) || !strncmp((char *)sfh.magic, "!Scream!", 8))
		libxmp_set_type(m, "Scream Tracker %d.%02d", sfh.vermaj, sfh.vermin);
	else if (!strncmp((char *)sfh.magic, "SWavePro", 8))
		libxmp_set_type(m, "SoundWave Pro %d.%02d", sfh.vermaj, sfh.vermin);
	else
		libxmp_copy_adjust(mod->type, sfh.magic, 8);

	MODULE_INFO();

	if (libxmp_init_instrument(m) < 0)
		return -1;

	/* Read and convert instruments and samples */
	for (i = 0; i < mod->ins; i++) {
		if (libxmp_alloc_subinstrument(mod, i, 1) < 0)
			return -1;

		mod->xxs[i].len = sfh.ins[i].length;
		mod->xxs[i].lps = sfh.ins[i].loopbeg;
		mod->xxs[i].lpe = sfh.ins[i].loopend;
		if (mod->xxs[i].lpe == 0xffff)
			mod->xxs[i].lpe = 0;
		mod->xxs[i].flg = mod->xxs[i].lpe > 0 ? XMP_SAMPLE_LOOP : 0;
		mod->xxi[i].sub[0].vol = sfh.ins[i].volume;
		mod->xxi[i].sub[0].pan = XMP_INST_NO_DEFAULT_PAN;
		mod->xxi[i].sub[0].sid = i;

		if (mod->xxs[i].len > 0)
			mod->xxi[i].nsm = 1;

		libxmp_instrument_name(mod, i, sfh.ins[i].name, 12);

		D_(D_INFO "[%2X] %-14.14s %04x %04x %04x %c V%02x %5d", i,
		   mod->xxi[i].name, mod->xxs[i].len, mod->xxs[i].lps,
		   mod->xxs[i].lpe,
		   mod->xxs[i].flg & XMP_SAMPLE_LOOP ? 'L' : ' ',
		   mod->xxi[i].sub[0].vol, sfh.ins[i].c2spd);

		libxmp_c2spd_to_note(sfh.ins[i].c2spd, &mod->xxi[i].sub[0].xpo,
			      &mod->xxi[i].sub[0].fin);
	}

	if (hio_read(mod->xxo, 1, mod->len, f) < mod->len)
		return -1;

	for (i = 0; i < mod->len; i++) {
		if (mod->xxo[i] >= 99) {
			break;
		}
		/* Patterns >= the pattern count are valid blank patterns.
		 * Examples: jimmy.stm, Rauno/dogs.stm, Skaven/hevijanis istu maas.stm.
		 * Patterns >= 64 have undefined behavior in Screamtracker 2.
		 */
		if (mod->xxo[i] >= mod->pat) {
			mod->xxo[i] = mod->pat;
			blank_pattern = 1;
		}
	}
	stored_patterns = mod->pat;
	if(blank_pattern)
		mod->pat++;

	mod->trk = mod->pat * mod->chn;
	mod->len = i;

	D_(D_INFO "Module length: %d", mod->len);

	if (libxmp_init_pattern(mod) < 0)
		return -1;

	/* Read and convert patterns */
	D_(D_INFO "Stored patterns: %d", stored_patterns);

	if(blank_pattern) {
		if (libxmp_alloc_pattern_tracks(mod, stored_patterns, 64) < 0)
			return -1;
	}

	for (i = 0; i < stored_patterns; i++) {
		if (libxmp_alloc_pattern_tracks(mod, i, 64) < 0)
			return -1;

		if (hio_error(f))
			return -1;

		for (j = 0; j < 64; j++) {
			for (k = 0; k < mod->chn; k++) {
				event = &EVENT(i, k, j);
				b = hio_read8(f);
				if (b == 251 || b == 252)
					continue; /* Empty note */

				if (b == 253) {
					event->note = XMP_KEY_OFF;
					continue;  /* Key off */
				}

				if (b == 254)
					event->note = XMP_KEY_OFF;
				else if (b == 255)
					event->note = 0;
				else
					event->note = 1 + LSN(b) + 12 * (3 + MSN(b));

				b = hio_read8(f);
				event->vol = b & 0x07;
				event->ins = (b & 0xf8) >> 3;

				b = hio_read8(f);
				event->vol += (b & 0xf0) >> 1;
				if (version >= 200) {
					event->vol = (event->vol > 0x40) ? 0 : event->vol + 1;
				} else {
					if (event->vol > 0) {
						event->vol = (event->vol > 0x40) ? 1 : event->vol + 1;
					}
				}

				event->fxt = fx[LSN(b)];
				event->fxp = hio_read8(f);
				switch (event->fxt) {
				case FX_BREAK:
					/* ST2 always breaks to row 0 */
					event->fxp = 0;
					break;
				case FX_SPEED:
					if (event->fxp) {
						stm_convert_tempo(&spd, &bpm,
							event->fxp, version);
						event->fxt = FX_S3M_SPEED;
						event->fxp = spd;
						if (version >= 110) {
							event->f2t = FX_S3M_BPM;
							event->f2p = bpm;
						}
					} else {
						/* A00 is a no-op */
						event->fxt = event->fxp = 0;
					}
					break;
				case FX_NONE:
					event->fxp = event->fxt = 0;
					break;
				}
			}
		}
	}

	/* Read samples */
	D_(D_INFO "Stored samples: %d", mod->smp);

	for (i = 0; i < mod->ins; i++) {
		if (!sfh.ins[i].volume || !sfh.ins[i].length) {
			mod->xxi[i].nsm = 0;
			continue;
		}

		if (sfh.type == STM_TYPE_SONG) {
			HIO_HANDLE *s;
			struct libxmp_path sp;
			char tmpname[32];
			const char *instname = mod->xxi[i].name;

			if (libxmp_copy_name_for_fopen(tmpname, instname, 32) != 0)
				continue;

			libxmp_path_init(&sp);
			if (libxmp_find_instrument_file(m, &sp, tmpname) != 0)
				continue;

			s = hio_open(sp.path, "rb");
			libxmp_path_free(&sp);
			if (s == NULL)
				continue;

			if (libxmp_load_sample(m, s, SAMPLE_FLAG_UNS, &mod->xxs[i], NULL) < 0) {
				hio_close(s);
				return -1;
			}
			hio_close(s);
		} else {
			hio_seek(f, start + (sfh.ins[i].rsvd1 << 4), SEEK_SET);
			if (libxmp_load_sample(m, f, 0, &mod->xxs[i], NULL) < 0)
				return -1;
		}
	}

	m->quirk |= QUIRK_VSALL | QUIRKS_ST3;
	m->flow_mode = FLOW_MODE_ST2;
	m->read_event_type = READ_EVENT_ST3;

	return 0;
}
