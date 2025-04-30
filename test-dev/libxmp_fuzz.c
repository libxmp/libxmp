/* Extended Module Player
 * Copyright (C) 2021-2024 Lachesis
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "../include/xmp.h"
#include "../src/common.h"
#include "../src/rng.h"

#define FIXED_SEED		0xbaadcafeUL
#define DEFAULT_FRAMES_TO_PLAY	64

#ifndef LIBXMP_LIBFUZZER
#include "../src/loaders/vorbis.h"

#define O_(...) do{ if(!quiet) { fprintf(stderr, "" __VA_ARGS__); fflush(stderr); }}while(0)

#ifdef LIBXMP_NO_PRINT_STATUS
#define DEFAULT_QUIET		1
#else
#define DEFAULT_QUIET		0
#endif

static int quiet = DEFAULT_QUIET;

static unsigned status_count[256];
static unsigned status_unkn = 0;
static unsigned total = 0;

static uint8_t *buf = NULL;
static size_t buf_sz = 0;
#endif

/* Desintations for position, row, time seeks. */
static const int seek_dests[] = {
	0, 16, 57, 255, 256, 32768, 1000000, INT_MAX,
	-1, -255, -32769, -1000000, INT_MIN
};

static inline int libxmp_test_function(xmp_context opaque, const uint8_t *data,
					size_t size, int frames_to_play)
{
#ifdef HAVE_FMEMOPEN
	FILE *f;
#endif
	int play_error = 0;
	int test_error = 0;
	int test_print = 0;
	int load_error;

	libxmp_set_random(&((struct context_data *)opaque)->rng, FIXED_SEED);

	/* Fuzz loaders. */
	load_error = xmp_load_module_from_memory(opaque, data, size);
	if (load_error == 0) {
		/* Fuzz playback. */
		struct xmp_module_info info;
		int interp, mono, i;

		/* Derive config from the MD5 for now... :( */
		xmp_get_module_info(opaque, &info);
		interp = info.md5[7] * 3U / 256;
		mono = (info.md5[3] & 1) ^ (info.md5[14] >> 7);

		switch (interp) {
		case 0:
			interp = XMP_INTERP_NEAREST;
			break;
		case 1:
			interp = XMP_INTERP_LINEAR;
			break;
		default:
			interp = XMP_INTERP_SPLINE;
			break;
		}
		xmp_start_player(opaque, XMP_MIN_SRATE, mono ? XMP_FORMAT_MONO : 0);
		xmp_set_player(opaque, XMP_PLAYER_INTERP, interp);

		/* Play initial frames */
		for (i = 0; i < frames_to_play; i++) {
			int r = xmp_play_frame(opaque);
			if (r != 0)
				play_error = r;
		}

		/* Relative positional seeks */
		xmp_next_position(opaque);
		xmp_play_frame(opaque);
		xmp_prev_position(opaque);
		xmp_play_frame(opaque);

		/* Relative positional seek to -1 */
		xmp_set_position(opaque, 0);
		xmp_play_frame(opaque);
		xmp_prev_position(opaque);
		xmp_play_frame(opaque);

		/* Relative positional seek to len */
		xmp_set_position(opaque, info.mod->len - 1);
		xmp_play_frame(opaque);
		xmp_next_position(opaque);
		xmp_play_frame(opaque);

		/* Restart module */
		xmp_restart_module(opaque);
		xmp_play_frame(opaque);
		xmp_restart_module(opaque);
		xmp_prev_position(opaque);
		xmp_play_frame(opaque);
		xmp_restart_module(opaque);
		xmp_next_position(opaque);
		xmp_play_frame(opaque);
		xmp_restart_module(opaque);
		xmp_set_position(opaque, 0);
		xmp_play_frame(opaque);

		/* Stop module */
		xmp_stop_module(opaque);
		xmp_play_frame(opaque); /* -XMP_END */
		xmp_prev_position(opaque);
		xmp_play_frame(opaque); /* -XMP_END */
		xmp_next_position(opaque);
		xmp_play_frame(opaque); /* ok */
		xmp_stop_module(opaque);
		xmp_restart_module(opaque);
		xmp_play_frame(opaque); /* ok */
		xmp_stop_module(opaque);
		xmp_set_position(opaque, 0);
		xmp_play_frame(opaque); /* ok */

		/* Absolute positional seeks */
		for (i = 0; i < (int)ARRAY_SIZE(seek_dests); i++) {
			xmp_set_position(opaque, seek_dests[i]);
			xmp_play_frame(opaque);
		}

		/* Row seeks within position 0 */
		for (i = 0; i < (int)ARRAY_SIZE(seek_dests); i++) {
			xmp_set_position(opaque, 0);
			xmp_set_row(opaque, seek_dests[i]);
			xmp_play_frame(opaque);
		}

		/* Time seeks */
		for (i = 0; i < (int)ARRAY_SIZE(seek_dests); i++) {
			xmp_seek_time(opaque, seek_dests[i]);
			xmp_play_frame(opaque);
		}

		/* TODO: other API functions? */

		xmp_release_module(opaque);
	}

#ifdef HAVE_FMEMOPEN
	/* Fuzz depackers. */
	f = fmemopen((void *)data, size, "rb");
	if (f != NULL) {
		struct xmp_test_info info;
		test_error = xmp_test_module_from_file(f, &info);
		test_print = 1;
		fclose(f);
	}
#endif

#ifndef LIBXMP_LIBFUZZER
	if (!quiet) {
		/* Print and log status */
		total++;
		if (load_error <= 0 && load_error > -256)
			status_count[-load_error]++;
		else
			status_unkn++;

		O_(" load:%d", load_error);
		if (test_print)
			O_(" test:%d", test_error);
		if (load_error == 0)
			O_(" play:%d", play_error);
	}
#endif
	return 0;
}

#ifdef LIBXMP_LIBFUZZER

/**
 * libFuzzer wrapper function.
 */
#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	xmp_context opaque = xmp_create_context();

	libxmp_test_function(opaque, data, size, DEFAULT_FRAMES_TO_PLAY);

	xmp_free_context(opaque);
	return 0;
}

#else /* !LIBXMP_LIBFUZZER */

static void test_file(xmp_context opaque, struct stat *st,
			const char *filename, int frames_to_play)
{
	FILE *f;
	size_t len;

	O_("  %s ...", filename);

	f = fopen(filename, "rb");
	if (f == NULL) {
		O_(" fopen error\n");
		return;
	}
	if (fstat(fileno(f), st) != 0) {
		O_(" fstat error\n");
		fclose(f);
		return;
	}
	len = st->st_size;
	if (len > buf_sz) {
		uint8_t *tmp = (uint8_t *)realloc(buf, len);
		if (tmp == NULL) {
			O_(" malloc error\n");
			fclose(f);
			return;
		}
		buf = tmp;
		buf_sz = len;
	}
	if (fread(buf, 1, len, f) != len) {
		O_(" fread error\n");
		fclose(f);
		return;
	}
	fclose(f);

	/* Special case: call stb-vorbis directly for Ogg signatures */
	if (len >= 4 && !memcmp(buf, "OggS", 4)) {
		int16_t *pcm16 = NULL;
		int ch, rate;

		int ret = stb_vorbis_decode_memory(buf, len,
			&ch, &rate, &pcm16);
		if (pcm16)
			free(pcm16);

		O_(" vorbis:%d\n", ret);
		return;
	}

	libxmp_test_function(opaque, buf, len, frames_to_play);

	O_("\n");
}

/**
 * Standalone tester. Status output can be suppressed by providing -q or
 * defining LIBXMP_NO_PRINT_STATUS, which might be useful for other fuzzers.
 */
int main(int argc, char **argv)
{
	xmp_context opaque;
	char path[1024];
	struct stat st;
	int frames_to_play = DEFAULT_FRAMES_TO_PLAY;
	int allow_opt = 1;
	int i;

	if (argc < 2)
		return 0;

	opaque = xmp_create_context();
	if (!opaque)
		return -1;

	/* stat instrumentation is broken for some versions of MSan */
	memset(&st, 0, sizeof(struct stat));

	for (i = 1; i < argc; i++) {
		if (allow_opt && argv[i][0] == '-') {
			if (!argv[i][1]) {
				while (fgets(path, sizeof(path), stdin)) {
					size_t l = strlen(path);
					while (l && (path[l - 1] == '\r' || path[l - 1] == '\n'))
						path[--l] = '\0';
					test_file(opaque, &st, path, frames_to_play);
				}
				continue;
			}
			if (argv[i][1] == '-' && !argv[i][2])
				allow_opt = 0;
			if (argv[i][1] == 'v' && !argv[i][2])
				quiet = 0;
			if (argv[i][1] == 'q' && !argv[i][2])
				quiet = 1;
			if (argv[i][1] == 'f')
				frames_to_play = strtoul(argv[i] + 2, NULL, 10);
			continue;
		}
		test_file(opaque, &st, argv[i], frames_to_play);
	}

	if (total > 1) {
		if (status_unkn)
			O_("status    ?: %u\n", status_unkn);

		for (i = 0; i < 256; i++) {
			if (status_count[i])
				O_("status %4d: %u\n", -i, status_count[i]);
		}
	}

	xmp_free_context(opaque);
	free(buf);
	return 0;
}

#endif /* !LIBXMP_LIBFUZZER */
