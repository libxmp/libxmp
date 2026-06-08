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

/*
 * A module packer created by Shaun Southern. Samples are stored in a
 * separate file. File prefixes are mfp for song and smp for samples. For
 * more information see http://www.exotica.org.uk/wiki/Magnetic_Fields_Packer
 */

#include "loader.h"
#include "../path.h"

static int mfp_test(HIO_HANDLE *, char *, const int);
static int mfp_load(struct module_data *, HIO_HANDLE *, const int);

const struct format_loader libxmp_loader_mfp = {
	"Magnetic Fields Packer",
	mfp_test,
	mfp_load
};

static int mfp_test(HIO_HANDLE *f, char *t, const int start)
{
	uint8 buf[384];
	int i, len, lps, lsz;

	if (HIO_HANDLE_TYPE(f) != HIO_HANDLE_TYPE_FILE)
		return -1;

	if (hio_read(buf, 1, 384, f) < 384)
		return -1;

	/* check restart byte */
	if (buf[249] != 0x7f)
		return -1;

	for (i = 0; i < 31; i++) {
		/* check size */
		len = readmem16b(buf + i * 8);
		if (len > 0x7fff)
			return -1;

		/* check finetune */
		if (buf[i * 8 + 2] & 0xf0)
			return -1;

		/* check volume */
		if (buf[i * 8 + 3] > 0x40)
			return -1;

		/* check loop start */
		lps = readmem16b(buf + i * 8 + 4);
		if (lps > len)
			return -1;

		/* check loop size */
		lsz = readmem16b(buf + i * 8 + 6);
		if (lps + lsz - 1 > len)
			return -1;

		if (len > 0 && lsz == 0)
			return -1;
	}

	if (buf[248] != readmem16b(buf + 378))
		return -1;

	if (readmem16b(buf + 378) != readmem16b(buf + 380))
		return -1;

	libxmp_read_title(f, t, 0);

	return 0;
}

struct mfp_map {
	uint16 offset;
	uint8  ord;
	uint8  chn;
};

static int mfp_map_compare(const void *A, const void *B)
{
	const struct mfp_map *a = (const struct mfp_map *)A;
	const struct mfp_map *b = (const struct mfp_map *)B;
	return (int)a->offset - b->offset;
}

static int mfp_load(struct module_data *m, HIO_HANDLE *f, const int start)
{
	struct xmp_module *mod = &m->mod;
	int i, j, k, x, y;
	struct xmp_event *event;
	struct libxmp_path sp;
	HIO_HANDLE *s;
	int size1 /*, size2*/;
	long pat_addr;
	uint8 buf[255 * 2 + 4];
	uint8 *mod_event;
	struct mfp_map *mapping, *current;
	uint16 *track_offs;

	LOAD_INIT();

	libxmp_set_type(m, "Magnetic Fields Packer");
	MODULE_INFO();

	mod->chn = 4;
	mod->ins = mod->smp = 31;

	if (libxmp_init_instrument(m) < 0)
		return -1;

	for (i = 0; i < 31; i++) {
		int loop_size;

		if (libxmp_alloc_subinstrument(mod, i, 1) < 0)
			return -1;

		mod->xxs[i].len = 2 * hio_read16b(f);
		mod->xxi[i].sub[0].fin = (int8)(hio_read8(f) << 4);
		mod->xxi[i].sub[0].vol = hio_read8(f);
		mod->xxs[i].lps = 2 * hio_read16b(f);
		loop_size = hio_read16b(f);

		mod->xxs[i].lpe = mod->xxs[i].lps + 2 * loop_size;
		mod->xxs[i].flg = loop_size > 1 ? XMP_SAMPLE_LOOP : 0;
		mod->xxi[i].sub[0].pan = XMP_INST_NO_DEFAULT_PAN;
		mod->xxi[i].sub[0].sid = i;
		mod->xxi[i].rls = 0xfff;

		if (mod->xxs[i].len > 0)
			mod->xxi[i].nsm = 1;

		D_(D_INFO "[%2X] %04x %04x %04x %c V%02x %+d",
			i, mod->xxs[i].len, mod->xxs[i].lps,
			mod->xxs[i].lpe,
			loop_size > 1 ? 'L' : ' ',
			mod->xxi[i].sub[0].vol, mod->xxi[i].sub[0].fin >> 4);
	}

	mod->len = mod->pat = hio_read8(f);
	hio_read8(f);		/* restart */

	/*
	for (i = 0; i < 128; i++) {
		mod->xxo[i] = hio_read8(f);
	}
	if (hio_error(f)) {
		return -1;
	}
	*/
	if (hio_seek(f, 128, SEEK_CUR)) {
		D_(D_CRIT "seek error at order list");
		return -1;
	}
	for (i = 0; i < mod->len; i++) {
		mod->xxo[i] = i;
	}

	/* Read and convert patterns */

	size1 = hio_read16b(f);
	/* size2 = */ hio_read16b(f);
	if (size1 > mod->len) {
		D_(D_CRIT "invalid track table length %d > %d", size1, mod->len);
		return -1;
	}

	/* What follows is a list of order (not pattern) track offsets.
	 * To avoid duplicating tracks, try to merge track offsets.
	 *
	 * It is probably possible to merge patterns to match the original
	 * order list, but not really worth the effort.
	 */
	mapping = (struct mfp_map *) calloc(size1 * 4, sizeof(struct mfp_map));
	track_offs = (uint16 *) malloc(size1 * 4 * sizeof(uint16));

	if (mapping == NULL || track_offs == NULL) {
		free(mapping);
		free(track_offs);
		return -1;
	}

	D_(D_INFO "pattern table @ %lxh", hio_tell(f));
	current = mapping;
	for (i = 0; i < size1; i++) {		/* Read pattern table */
		for (j = 0; j < 4; j++) {
			current->offset = hio_read16b(f);
			current->ord = i;
			current->chn = j;
			D_(D_INFO "  %d,%d: %d", i, j, current->offset);
			current++;
		}
	}

	qsort(mapping, size1 * 4, sizeof(struct mfp_map), mfp_map_compare);

	mod->trk = 0;
	for (i = 0; i < size1 * 4; ) {
		int offset = mapping[i].offset;
		track_offs[mod->trk] = offset;

		while (i < size1 * 4 && mapping[i].offset == offset) {
			D_(D_INFO "  %d,%d (%d) -> track %d", mapping[i].ord,
			   mapping[i].chn, mapping[i].offset, mod->trk);

			/* Write track # to offset to save an extra field. */
			mapping[i].offset = mod->trk;
			i++;
		}
		mod->trk++;
	}

	if (libxmp_init_pattern(mod) < 0) {
		free(mapping);
		free(track_offs);
		return -1;
	}

	for (i = 0; i < mod->len; i++) {
		if (libxmp_alloc_pattern(mod, i) < 0) {
			free(mapping);
			free(track_offs);
			return -1;
		}
		mod->xxp[i]->rows = 64;
	}

	for (i = 0; i < size1 * 4; i++) {
		/* offset was replaced with the track number. */
		mod->xxp[mapping[i].ord]->index[mapping[i].chn] = mapping[i].offset;
	}
	free(mapping);

	D_(D_INFO "Stored tracks: %d ", mod->trk);

	pat_addr = hio_tell(f);
	D_(D_INFO "  @ %lxh", pat_addr);

	for (i = 0; i < mod->trk; i++) {
		int len;

		if (libxmp_alloc_track(mod, i, 64) < 0) {
			free(track_offs);
			return -1;
		}

		if (hio_seek(f, pat_addr + track_offs[i], SEEK_SET) < 0) {
			D_(D_CRIT "seek error to track %d", i);
			free(track_offs);
			return -1;
		}

		len = (i < mod->trk - 1) ? track_offs[i + 1] - track_offs[i]
					 : sizeof(buf);

		len = hio_read(buf, 1, MIN(len, sizeof(buf)), f);

		event = mod->xxt[i]->event;
		for (k = 0; k < 4; k++) {
			for (x = 0; x < 4; x++) {
				for (y = 0; y < 4; y++) {
					if (k >= len ||
					    buf[k] + x >= len ||
					    buf[buf[k] + x] + y >= len ||
					    buf[buf[buf[k] + x] + y] * 2 + 4 > len) {
						D_(D_CRIT "read error at track %d", i);
						free(track_offs);
						return -1;
					}
					mod_event = &buf[buf[buf[buf[k] + x] + y] * 2];
					libxmp_decode_protracker_event(event, mod_event);
					event++;
				}
			}
		}
	}
	free(track_offs);

	/* Read samples */
	D_(D_INFO "Loading samples: %d", mod->ins);

	libxmp_path_init(&sp);

	/* first check smp.filename */
	if (strlen(m->basename) < 5 || m->basename[3] != '.') {
		D_(D_CRIT "invalid filename %s", m->basename);
		goto err;
	}

	m->basename[0] = 's';
	m->basename[1] = 'm';
	m->basename[2] = 'p';

	if (libxmp_path_join(&sp, m->dirname, m->basename) != 0) {
		D_(D_CRIT "failed to join path %s", m->basename);
		goto err;
	}

	if ((s = hio_open(sp.path, "rb")) == NULL) {
		/* handle .set filenames like in Kid Chaos*/
		if (strchr(m->basename, '-')) {
			char *p = strrchr(sp.path, '-');
			if (p != NULL) {
				size_t ppos = p - sp.path;
				if (libxmp_path_suffix_at(&sp, ppos, ".set") != 0) {
					D_(D_CRIT "failed to append .set");
					goto err;
				}
			}
		}
		if ((s = hio_open(sp.path, "rb")) == NULL) {
			D_(D_CRIT "can't open sample file %s", sp.path);
			goto err;
		}
	}
	libxmp_path_free(&sp);

	for (i = 0; i < mod->ins; i++) {
		if (libxmp_load_sample(m, s, SAMPLE_FLAG_FULLREP,
				&mod->xxs[mod->xxi[i].sub[0].sid], NULL) < 0) {
			free(s);
			return -1;
		}
	}

	hio_close(s);

	m->period_type = PERIOD_MODRNG;

	return 0;

    err:
	libxmp_path_free(&sp);
	for (i = 0; i < mod->ins; i++) {
		mod->xxi[i].nsm = 0;
		memset(&mod->xxs[i], 0, sizeof(struct xmp_sample));
	}

	return 0;
}
