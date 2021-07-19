/* Extended Module Player
 * Copyright (C) 1996-2021 Claudio Matsuoka and Hipolito Carraro Jr
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

#include "../common.h"
#include "depacker.h"
#include "miniz.h"

static int test_muse(unsigned char *b)
{
	if (memcmp(b, "MUSE", 4) == 0) {
		uint32 r = readmem32b(b + 4);
		/* MOD2J2B uses 0xdeadbabe */
		if (r == 0xdeadbeaf || r == 0xdeadbabe) {
			return 1;
		}
	}

	return 0;
}

static int tinfl_put_buf_func(const void* pBuf, int len, void *pUser)
{
	return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
}

static int decrunch_muse(HIO_HANDLE *f, FILE *fo, long inlen)
{
	size_t in_buf_size = inlen - 24;
	uint8 *pCmp_data;

	if (hio_seek(f, 24, SEEK_SET) < 0) {
		D_(D_CRIT "hio_seek() failed");
		return -1;
	}

	pCmp_data = (uint8 *)malloc(in_buf_size);
	if (!pCmp_data) {
		D_(D_CRIT "Out of memory");
		return -1;
	}

	if (hio_read(pCmp_data, 1, in_buf_size, f) != in_buf_size) {
		D_(D_CRIT "Failed reading input file");
		free(pCmp_data);
		return -1;
	}

	if (tinfl_decompress_mem_to_callback(pCmp_data, &in_buf_size, tinfl_put_buf_func, fo, TINFL_FLAG_PARSE_ZLIB_HEADER) == 0) {
		D_(D_CRIT "tinfl_decompress_mem_to_callback() failed");
		free(pCmp_data);
		return -1;
	}

	free(pCmp_data);
	return 0;
}

struct depacker libxmp_depacker_muse = {
	test_muse,
	decrunch_muse
};
