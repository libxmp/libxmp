/* Extended Module Player
 * Copyright (C) 2024-2025 Alice Rowan <petrifiedrowan@gmail.com>
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

/* Pack-Ice depacker. */

#include "depacker.h"
#include "ice_unpack.h"

#include <limits.h>

static size_t libxmp_ice_read_fn(void * ICE_RESTRICT dest, size_t num, void *priv)
{
	return hio_read(dest, 1, num, (HIO_HANDLE *)priv);
}

static int libxmp_ice_seek_fn(void *priv, ice_int64 offset, int whence)
{
	/* TODO: hio_seek should guarantee a 64-bit range for offset.
	 * There are no >2GB Pack-Ice files, so this isn't important here... */
	return hio_seek((HIO_HANDLE *)priv, (long)offset, whence);
}

static int ice_check_lengths(ice_int64 outlen, long inlen)
{
	if (outlen <= 0 || outlen > LIBXMP_DEPACK_LIMIT) {
		return -1;
	}
	/* TODO: limiting inlen to INT_MAX, see libxmp_ice_seek_fn.
	 * Actual limit determined by file format is UINT32_MAX. */
	if (inlen < 0 || inlen >= INT_MAX) {
		return -1;
	}
	if (outlen > ice_uncompressed_bound((ice_uint32)inlen)) {
		return -1;
	}
	return 0;
}


static int ice1_test(HIO_HANDLE *in)
{
	uint8 tmp[8];

	if (hio_seek(in, -8, SEEK_END) < 0) {
		return 0;
	}
	if (hio_read(tmp, 1, 8, in) < 8) {
		return 0;
	}

	return ice1_unpack_test(tmp, 8) >= 0;
}

static int ice1_decrunch(HIO_HANDLE *in, void **out, long *outlen)
{
	uint8 tmp[8];
	void *buf;
	long inlen;
	ice_int64 sz;

	if (hio_seek(in, -8, SEEK_END) < 0) {
		return -1;
	}
	if (hio_read(tmp, 1, 8, in) < 8) {
		return -1;
	}
	inlen = hio_size(in);

	sz = ice1_unpack_test(tmp, 8);
	if (ice_check_lengths(sz, inlen) < 0) {
		return -1;
	}
	if ((buf = malloc((size_t)sz)) == NULL) {
		return -1;
	}

	if (ice1_unpack(buf, (size_t)sz,
	    libxmp_ice_read_fn, libxmp_ice_seek_fn, in, (ice_uint32)inlen) < 0) {
		free(buf);
		return -1;
	}
	*out = buf;
	*outlen = sz;
	return 0;
}

const struct depacker libxmp_depacker_ice1 =
{
	NULL,
	ice1_test,
	ice1_decrunch
};


static int ice2_test(unsigned char *data)
{
	return ice2_unpack_test(data, 12) >= 0;
}

static int ice2_decrunch(HIO_HANDLE *in, void **out, long *outlen)
{
	uint8 tmp[12];
	void *buf;
	long inlen;
	ice_int64 sz;

	if (hio_seek(in, 0, SEEK_SET) < 0) {
		return -1;
	}
	if (hio_read(tmp, 1, 12, in) < 12) {
		return -1;
	}
	inlen = hio_size(in);

	sz = ice2_unpack_test(tmp, 12);
	if (ice_check_lengths(sz, inlen) < 0) {
		return -1;
	}
	if ((buf = malloc((size_t)sz)) == NULL) {
		return -1;
	}

	if (ice2_unpack(buf, (size_t)sz,
	    libxmp_ice_read_fn, libxmp_ice_seek_fn, in, (ice_uint32)inlen) < 0) {
		free(buf);
		return -1;
	}
	*out = buf;
	*outlen = sz;
	return 0;
}

const struct depacker libxmp_depacker_ice2 =
{
	ice2_test,
	NULL,
	ice2_decrunch
};
