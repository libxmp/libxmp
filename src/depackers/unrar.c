/* Extended Module Player
 * Copyright (C) 1996-2022 Claudio Matsuoka and Hipolito Carraro Jr
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

#include "depacker.h"

#define RAR_MAX_OUTPUT_SIZE (1 << 29)

#ifdef HAVE_LIBUNARR
#include <unarr.h>
#endif

static int test_rar(unsigned char *data)
{
	/* The bytes afterward may vary depending on the RAR version. */
	return !memcmp(data, "Rar!\x1a\x07", 6);
}

static int decrunch_rar(HIO_HANDLE *in, void **out, long inlen, long *outlen)
{
	int ret = -1;
#ifdef HAVE_LIBUNARR
	void *in_buf = NULL;
	ar_stream *stream = NULL;
	ar_archive *rar = NULL;

	const char *filename;
	void *out_buf;

	if (in->type != HIO_HANDLE_TYPE_MEMORY) {
		/* No FILE * or callbacks-based API functions yet. */
		if ((in_buf = malloc(inlen)) == NULL)
			return -1;

		if (hio_read(in_buf, 1, inlen, in) < inlen)
			goto err;
	} else {
		in_buf = in->handle.mem;
	}

	if ((stream = ar_open_memory(in_buf, inlen)) == NULL)
		goto err;

	if ((rar = ar_open_rar_archive(stream)) == NULL) {
		D_(D_CRIT "failed to open RAR archive");
		goto err;
	}

	while (ar_parse_entry(rar)) {
		long size = ar_entry_get_size(rar);

		if (size <= 0 || size > RAR_MAX_OUTPUT_SIZE) {
			D_(D_INFO "Skipping unsupported size %ld", size);
			continue;
		}

		filename = ar_entry_get_name(rar);
		if (libxmp_exclude_match(filename)) {
			D_(D_INFO "Skipping file %s", filename);
			continue;
		}

		if ((out_buf = malloc(size)) == NULL)
			goto err;

		if (!ar_entry_uncompress(rar, out_buf, size)) {
			D_(D_CRIT "uncompress error");
			free(out_buf);
			goto err;
		}

		*out = out_buf;
		*outlen = size;
		ret = 0;
		break;
	}

    err:
	if (rar)
		ar_close_archive(rar);
	if (stream)
		ar_close(stream);

	if (in->type != HIO_HANDLE_TYPE_MEMORY)
		free(in_buf);
#endif
	return ret;
}

struct depacker libxmp_depacker_rar = {
	test_rar,
	NULL,
	decrunch_rar
};
