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

#include <errno.h>
#include "common.h"
#include "hio.h"
#include "callbackio.h"
#include "mdataio.h"

static long get_size(FILE *f)
{
	long size, pos;

	pos = ftell(f);
	if (pos >= 0) {
		if (fseek(f, 0, SEEK_END) < 0) {
			return -1;
		}
		size = ftell(f);
		if (fseek(f, pos, SEEK_SET) < 0) {
			return -1;
		}
		return size;
	} else {
		return pos;
	}
}

static size_t hio_read_internal(void *buf, size_t size, size_t num, HIO_HANDLE *h)
{
	size_t ret = 0;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = fread(buf, size, num, h->handle.file);
		if (ret != num) {
			if (ferror(h->handle.file)) {
				h->error = errno;
			} else {
				h->error = feof(h->handle.file) ? EOF : -2;
			}
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mread(buf, size, num, h->handle.mem);
		if (ret != num) {
			h->error = EOF;
		}
		break;
	case HIO_HANDLE_TYPE_CBFILE:
		ret = cbread(buf, size, num, h->handle.cbfile);
		if (ret != num) {
			h->error = EOF;
		}
		break;
	}

	return ret;
}

static int hio_seek_internal(HIO_HANDLE *h, long offset, int whence)
{
	int ret = -1;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = fseek(h->handle.file, offset, whence);
		if (ret < 0) {
			h->error = errno;
		}
		else if (h->error == EOF) {
			h->error = 0;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mseek(h->handle.mem, offset, whence);
		if (ret < 0) {
			h->error = EINVAL;
		}
		else if (h->error == EOF) {
			h->error = 0;
		}
		break;
	case HIO_HANDLE_TYPE_CBFILE:
		ret = cbseek(h->handle.cbfile, offset, whence);
		if (ret < 0) {
			h->error = EINVAL;
		}
		else if (h->error == EOF) {
			h->error = 0;
		}
		break;
	}

	return ret;
}

static long hio_tell_internal(HIO_HANDLE *h)
{
	long ret = -1;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = ftell(h->handle.file);
		if (ret < 0) {
			h->error = errno;
		}
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mtell(h->handle.mem);
		if (ret < 0) {
		/* should _not_ happen! */
			h->error = EINVAL;
		}
		break;
	case HIO_HANDLE_TYPE_CBFILE:
		ret = cbtell(h->handle.cbfile);
		if (ret < 0) {
			h->error = EINVAL;
		}
		break;
	}

	return ret;
}

static int hio_eof_internal(HIO_HANDLE *h)
{
	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		return feof(h->handle.file);
	case HIO_HANDLE_TYPE_MEMORY:
		return meof(h->handle.mem);
	case HIO_HANDLE_TYPE_CBFILE:
		return cbeof(h->handle.cbfile);
	}
	return EOF;
}

static void hio_fill_buffer_internal(HIO_HANDLE *h)
{
	if (h->buffer_end == HIO_FULL_BUFFER) {
		/* copy old "preview" bytes */
		h->buffer[0] = h->buffer[HIO_BUFFER_SIZE + 0];
		h->buffer[1] = h->buffer[HIO_BUFFER_SIZE + 1];
		h->buffer[2] = h->buffer[HIO_BUFFER_SIZE + 2];
		h->buffer[3] = h->buffer[HIO_BUFFER_SIZE + 3];

		h->buffer_end = 4 + hio_read_internal(h->buffer + 4, 1, HIO_BUFFER_SIZE, h);
	}
	else {
		/* read a full buffer from file */
		h->buffer_end = hio_read_internal(h->buffer, 1, HIO_FULL_BUFFER, h);
	}

	/* don't set EOF error yet */
	if(h->error == EOF) {
		h->error = 0;
	}
}

int8 hio_read8s(HIO_HANDLE *h)
{
	int8 ret = 0;

	if (HIO_BUFFER_CURSOR(h) >= h->buffer_end) {
		h->error = EOF;
		return ret;
	}

	ret = (int8)h->buffer[HIO_BUFFER_CURSOR(h)];
	h->cursor++;

	if (HIO_BUFFER_CURSOR(h) < 1) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint8 hio_read8(HIO_HANDLE *h)
{
	uint8 ret = 0;

	if (HIO_BUFFER_CURSOR(h) >= h->buffer_end) {
		h->error = EOF;
		return ret;
	}

	ret = h->buffer[HIO_BUFFER_CURSOR(h)];
	h->cursor++;

	if (HIO_BUFFER_CURSOR(h) < 1) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint16 hio_read16l(HIO_HANDLE *h)
{
	uint16 ret = 0;
	uint8 buf[2];

	if (HIO_BUFFER_CURSOR(h) + 1 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	h->cursor += 2;

	ret = readmem16l(buf);

	if (HIO_BUFFER_CURSOR(h) < 2) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint16 hio_read16b(HIO_HANDLE *h)
{
	uint16 ret = 0;
	uint8 buf[2];

	if (HIO_BUFFER_CURSOR(h) + 1 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	h->cursor += 2;

	ret = readmem16b(buf);

	if (HIO_BUFFER_CURSOR(h) < 2) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint32 hio_read24l(HIO_HANDLE *h)
{
	uint32 ret = 0;
	uint8 buf[3];

	if (HIO_BUFFER_CURSOR(h) + 2 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	buf[2] = h->buffer[HIO_BUFFER_CURSOR(h) + 2];
	h->cursor += 3;

	ret = readmem24l(buf);

	if (HIO_BUFFER_CURSOR(h) < 3) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint32 hio_read24b(HIO_HANDLE *h)
{
	uint32 ret = 0;
	uint8 buf[3];

	if (HIO_BUFFER_CURSOR(h) + 2 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	buf[2] = h->buffer[HIO_BUFFER_CURSOR(h) + 2];
	h->cursor += 3;

	ret = readmem24b(buf);

	if (HIO_BUFFER_CURSOR(h) < 3) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint32 hio_read32l(HIO_HANDLE *h)
{
	uint32 ret = 0;
	uint8 buf[4];

	if (HIO_BUFFER_CURSOR(h) + 3 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	buf[2] = h->buffer[HIO_BUFFER_CURSOR(h) + 2];
	buf[3] = h->buffer[HIO_BUFFER_CURSOR(h) + 3];
	h->cursor += 4;

	ret = readmem32l(buf);

	if (HIO_BUFFER_CURSOR(h) < 4) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

uint32 hio_read32b(HIO_HANDLE *h)
{
	uint32 ret = 0;
	uint8 buf[4];

	if (HIO_BUFFER_CURSOR(h) + 3 >= h->buffer_end) {
		h->cursor += h->buffer_end - HIO_BUFFER_CURSOR(h);
		h->error = EOF;
		return ret;
	}

	buf[0] = h->buffer[HIO_BUFFER_CURSOR(h)];
	buf[1] = h->buffer[HIO_BUFFER_CURSOR(h) + 1];
	buf[2] = h->buffer[HIO_BUFFER_CURSOR(h) + 2];
	buf[3] = h->buffer[HIO_BUFFER_CURSOR(h) + 3];
	h->cursor += 4;

	ret = readmem32b(buf);

	if (HIO_BUFFER_CURSOR(h) < 4) {
		hio_fill_buffer_internal(h);
	}

	return ret;
}

size_t hio_read(void *buf, size_t size, size_t num, HIO_HANDLE *h)
{
	/* memcpy the current HIO buffer, then read directly into buf, then copy back to the HIO buffer and finish it */
	size_t should_read = size * num;
	int buffer_left = h->buffer_end - HIO_BUFFER_CURSOR(h);
	size_t did_read;

	if(should_read <= buffer_left) {
		/* easy case */
		memcpy(buf, h->buffer + HIO_BUFFER_CURSOR(h), size * num);

		h->cursor += size * num;
		if(HIO_BUFFER_CURSOR(h) < should_read) {
			hio_fill_buffer_internal(h);
		}

		return should_read;
	}

	/* memcpy the current buffer */
	memcpy(buf, h->buffer + HIO_BUFFER_CURSOR(h), buffer_left);
	buf += buffer_left;
	should_read -= buffer_left;
	h->cursor += buffer_left;

	/* read the rest directly, may set h->error */
	did_read = hio_read_internal(buf, 1, should_read, h);
	h->cursor += did_read;

	/* copy back to the HIO buffer */
	memcpy(h->buffer, buf + did_read - HIO_BUFFER_CURSOR(h), HIO_BUFFER_CURSOR(h));
	h->buffer_end = HIO_BUFFER_CURSOR(h);

	/* finish the HIO buffer */
	if(did_read == should_read) {
		h->buffer_end += hio_read_internal(h->buffer + HIO_BUFFER_CURSOR(h), 1, HIO_BUFFER_SIZE - HIO_BUFFER_CURSOR(h), h);
	}

	return (buffer_left + did_read) / size;
}

int hio_seek(HIO_HANDLE *h, long offset, int whence)
{
	int ret;
	int seek_error;
	ptrdiff_t ofs = offset;

	/* special case for SEEK_END, will need to rewind to fill buffer */
	if (whence == SEEK_END) {
		ret = hio_seek_internal(h, offset, whence);

		if (ret < 0) {
			return ret;
		}

		ofs = hio_tell_internal(h);
	}

	if (whence == SEEK_CUR) {
		ofs += h->cursor;
	}

	if (ofs < 0) return -1;

	if (whence != SEEK_END && ofs / HIO_BUFFER_SIZE == h->cursor / HIO_BUFFER_SIZE) {
		h->cursor = ofs;
		return 0;
	}

	h->cursor = ofs;

	/* round to buffer start */
	ofs = (ofs / HIO_BUFFER_SIZE) * HIO_BUFFER_SIZE;

	ret = hio_seek_internal(h, ofs, SEEK_SET);
	seek_error = h->error;

	h->buffer_end = hio_read_internal(h->buffer, 1, HIO_FULL_BUFFER, h);

	/* don't set EOF error yet */
	if(h->error == EOF) {
		h->error = seek_error;
	}

	return ret;
}

long hio_tell(HIO_HANDLE *h)
{
	return h->cursor;
}

int hio_eof(HIO_HANDLE *h)
{
	if (HIO_BUFFER_CURSOR(h) < h->buffer_end)
		return 0;

	return hio_eof_internal(h);
}

int hio_error(HIO_HANDLE *h)
{
	int error = h->error;
	h->error = 0;
	return error;
}

HIO_HANDLE *hio_open(const char *path, const char *mode)
{
	HIO_HANDLE *h;

	h = (HIO_HANDLE *) calloc(1, sizeof(HIO_HANDLE));
	if (h == NULL)
		goto err;

	h->type = HIO_HANDLE_TYPE_FILE;
	h->handle.file = fopen(path, mode);
	if (h->handle.file == NULL)
		goto err2;

	h->size = get_size(h->handle.file);
	if (h->size < 0)
		goto err3;

	h->cursor = 0;
	h->buffer_end = 0;
	hio_fill_buffer_internal(h);

	return h;

    err3:
	fclose(h->handle.file);
    err2:
	free(h);
    err:
	return NULL;
}

HIO_HANDLE *hio_open_mem(const void *ptr, long size, int free_after_use)
{
	HIO_HANDLE *h;

	if (size <= 0) return NULL;
	h = (HIO_HANDLE *) calloc(1, sizeof(HIO_HANDLE));
	if (h == NULL)
		return NULL;

	h->type = HIO_HANDLE_TYPE_MEMORY;
	h->handle.mem = mopen(ptr, size, free_after_use);
	h->size = size;

	if (!h->handle.mem) {
		free(h);
		h = NULL;
	}

	h->cursor = 0;
	h->buffer_end = 0;
	hio_fill_buffer_internal(h);

	return h;
}

HIO_HANDLE *hio_open_file(FILE *f)
{
	HIO_HANDLE *h;

	h = (HIO_HANDLE *) calloc(1, sizeof(HIO_HANDLE));
	if (h == NULL)
		return NULL;

	h->noclose = 1;
	h->type = HIO_HANDLE_TYPE_FILE;
	h->handle.file = f;
	h->size = get_size(f);
	if (h->size < 0) {
		free(h);
		return NULL;
	}

	h->cursor = -HIO_FULL_BUFFER;
	h->buffer_end = 0;
	hio_seek(h, ftell(f), SEEK_SET);

	return h;
}

HIO_HANDLE *hio_open_file2(FILE *f)
{
	HIO_HANDLE *h = hio_open_file(f);
	if (h != NULL) {
		h->noclose = 0;
	}
	else {
		fclose(f);
	}
	return h;
}

HIO_HANDLE *hio_open_callbacks(void *priv, struct xmp_callbacks callbacks)
{
	HIO_HANDLE *h;
	CBFILE *f = cbopen(priv, callbacks);
	if (!f)
		return NULL;

	h = (HIO_HANDLE *) calloc(1, sizeof(HIO_HANDLE));
	if (h == NULL) {
		cbclose(f);
		return NULL;
	}

	h->type = HIO_HANDLE_TYPE_CBFILE;
	h->handle.cbfile = f;
	h->size = cbfilelength(f);
	if (h->size < 0) {
		cbclose(f);
		free(h);
		return NULL;
	}

	h->cursor = 0;
	h->buffer_end = 0;
	hio_fill_buffer_internal(h);

	return h;
}

static int hio_close_internal(HIO_HANDLE *h)
{
	int ret = -1;

	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
		ret = (h->noclose)? 0 : fclose(h->handle.file);
		break;
	case HIO_HANDLE_TYPE_MEMORY:
		ret = mclose(h->handle.mem);
		break;
	case HIO_HANDLE_TYPE_CBFILE:
		ret = cbclose(h->handle.cbfile);
		break;
	}
	return ret;
}

/* hio_close + hio_open_mem. Reuses the same HIO_HANDLE. */
int hio_reopen_mem(const void *ptr, long size, int free_after_use, HIO_HANDLE *h)
{
	MFILE *m;
	int ret;
	if (size <= 0) return -1;

	m = mopen(ptr, size, free_after_use);
	if (m == NULL) {
		return -1;
	}

	ret = hio_close_internal(h);
	if (ret < 0) {
		m->free_after_use = 0;
		mclose(m);
		return ret;
	}

	h->type = HIO_HANDLE_TYPE_MEMORY;
	h->handle.mem = m;
	h->size = size;

	h->cursor = 0;
	h->buffer_end = 0;
	hio_fill_buffer_internal(h);

	return 0;
}

/* hio_close + hio_open_file. Reuses the same HIO_HANDLE. */
int hio_reopen_file(FILE *f, int close_after_use, HIO_HANDLE *h)
{
	long size = get_size(f);
	int ret;
	if (size < 0) {
		return -1;
	}

	ret = hio_close_internal(h);
	if (ret < 0) {
		return -1;
	}

	h->noclose = !close_after_use;
	h->type = HIO_HANDLE_TYPE_FILE;
	h->handle.file = f;
	h->size = size;

	h->cursor = -HIO_FULL_BUFFER;
	h->buffer_end = 0;
	hio_seek(h, ftell(f), SEEK_SET);

	return 0;
}

int hio_close(HIO_HANDLE *h)
{
	int ret = hio_close_internal(h);
	free(h);
	return ret;
}

long hio_size(HIO_HANDLE *h)
{
	return h->size;
}

/* Returns a pointer to the underlying continuous memory buffer the entire
 * contents of HIO_HANDLE `h` are stored at if applicable, otherwise NULL.
 * Do not reallocate this pointer or modify its underlying data!
 */
const unsigned char *hio_get_underlying_memory(HIO_HANDLE *h)
{
	switch (HIO_HANDLE_TYPE(h)) {
	case HIO_HANDLE_TYPE_FILE:
	case HIO_HANDLE_TYPE_CBFILE:
		return NULL;
	case HIO_HANDLE_TYPE_MEMORY:
		return h->handle.mem->start;
	}
	return NULL;
}
