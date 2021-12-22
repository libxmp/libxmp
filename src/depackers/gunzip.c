/* Extended Module Player
 * Copyright (C) 1996-2021 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * This file is part of the Extended Module Player and is distributed
 * under the terms of the GNU Lesser General Public License. See COPYING.LIB
 * for more information.
 */

#include "../common.h"
#include "depacker.h"
#include "miniz.h"

/* See RFC1952 for further information */

/* The flag byte is divided into individual bits as follows:

   bit 0   FTEXT
   bit 1   FHCRC
   bit 2   FEXTRA
   bit 3   FNAME
   bit 4   FCOMMENT
   bit 5   reserved
   bit 6   reserved
   bit 7   reserved
*/

#define FLAG_FTEXT	(1 << 0)
#define FLAG_FHCRC	(1 << 1)
#define FLAG_FEXTRA	(1 << 2)
#define FLAG_FNAME	(1 << 3)
#define FLAG_FCOMMENT	(1 << 4)

struct member {
	uint8 id1;
	uint8 id2;
	uint8 cm;
	uint8 flg;
	uint32 mtime;
	uint8 xfl;
	uint8 os;

	uint32 crc32;
	uint32 size;
};

static int test_gzip(unsigned char *b)
{
	return b[0] == 31 && b[1] == 139;
}

static int tinfl_put_buf_func(const void* pBuf, int len, void *pUser)
{
	return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
}

static int decrunch_gzip(HIO_HANDLE *in, FILE *out, long inlen)
{
	struct member member;
	int val, c;
	size_t in_buf_size;
	uint8 *pCmp_data;
	long start, end;

	member.id1 = hio_read8(in);
	member.id2 = hio_read8(in);
	member.cm  = hio_read8(in);
	member.flg = hio_read8(in);
	member.mtime = hio_read32l(in);
	member.xfl = hio_read8(in);
	member.os  = hio_read8(in);

	if (member.cm != 0x08) {
		D_(D_CRIT "Unsuported compression method: %x", member.cm);
		return -1;
	}

	if (member.flg & FLAG_FEXTRA) {
		int xlen = hio_read16l(in);
		if (hio_seek(in, xlen, SEEK_CUR) < 0) {
			D_(D_CRIT "hio_seek() failed");
			return -1;
		}
	}

	if (member.flg & FLAG_FNAME) {
		do {
			c = hio_read8(in);
			if (hio_error(in)) {
				D_(D_CRIT "hio_read8() failed");
				return -1;
			}
		} while (c != 0);
	}

	if (member.flg & FLAG_FCOMMENT) {
		do {
			c = hio_read8(in);
			if (hio_error(in)) {
				D_(D_CRIT "hio_read8() failed");
				return -1;
			}
		} while (c != 0);
	}

	if (member.flg & FLAG_FHCRC) {
		hio_read16l(in);
	}

	start = hio_tell(in);
	end = inlen - 8;
	in_buf_size = end - start;

	pCmp_data = (uint8 *)malloc(in_buf_size);
	if (!pCmp_data)
	{
		D_(D_CRIT "Out of memory");
		return -1;
	}

	if (hio_read(pCmp_data, 1, in_buf_size, in) != in_buf_size)
	{
		D_(D_CRIT "Failed reading input file");
		free(pCmp_data);
		return -1;
	}

	if (tinfl_decompress_mem_to_callback(pCmp_data, &in_buf_size, tinfl_put_buf_func, out, 0) == 0) {
		D_(D_CRIT "tinfl_decompress_mem_to_callback() failed");
		free(pCmp_data);
		return -1;
	}

	free(pCmp_data);

	/* TODO: Check CRC32 */
	val = hio_read32l(in);

	/* Check file size */
	val = hio_read32l(in);
	if (val != ftell(out)) {
		D_(D_CRIT "Invalid file size");
		return -1;
	}

	return 0;
}

struct depacker libxmp_depacker_gzip = {
	test_gzip,
	decrunch_gzip
};
