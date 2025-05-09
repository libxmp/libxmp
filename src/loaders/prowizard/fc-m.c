/* ProWizard
 * Copyright (C) 1997 Asle / ReDoX
 * Modified in 2006,2007,2014 by Claudio Matsuoka
 * Modified in 2020 by Alice Rowan
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
 * FC-M_Packer.c
 *
 * Converts back to ptk FC-M packed MODs
 */

#include "prowiz.h"


static int depack_fcm(HIO_HANDLE *in, FILE *out)
{
	uint8 c1;
	uint8 ptable[128];
	uint8 pat_pos;
	uint8 pat_max;
	int i;
	int size, ssize = 0;

	memset(ptable, 0, sizeof(ptable));

	hio_read32b(in);				/* bypass "FC-M" ID */
	hio_read16b(in);				/* version number? */
	hio_read32b(in);				/* bypass "NAME" chunk */
	pw_move_data(out, in, 20);		/* read and write title */
	hio_read32b(in);				/* bypass "INST" chunk */

	/* read and write sample descriptions */
	for (i = 0; i < 31; i++) {
		pw_write_zero(out, 22);		/*sample name */
		write16b(out, size = hio_read16b(in));	/* size */
		ssize += size * 2;
		write8(out, hio_read8(in));		/* finetune */
		write8(out, hio_read8(in));		/* volume */
		write16b(out, hio_read16b(in));	/* loop start */
		size = hio_read16b(in);		/* loop size */
		if (size == 0)
			size = 1;
		write16b(out, size);
	}

	hio_read32b(in);				/* bypass "LONG" chunk */
	write8(out, pat_pos = hio_read8(in));	/* pattern table length */
	write8(out, hio_read8(in));			/* NoiseTracker byte */
	hio_read32b(in);				/* bypass "PATT" chunk */

	/* read and write pattern list and get highest patt number */
	for (pat_max = i = 0; i < pat_pos; i++) {
		write8(out, c1 = hio_read8(in));
		if (c1 > pat_max)
			pat_max = c1;
	}
	for (; i < 128; i++)
		write8(out, 0);

	write32b(out, PW_MOD_MAGIC);		/* write ptk ID */
	hio_read32b(in);				/* bypass "SONG" chunk */

	for (i = 0; i <= pat_max; i++)		/* pattern data */
		pw_move_data(out, in, 1024);

	hio_read32b(in);				/* bypass "SAMP" chunk */
	pw_move_data(out, in, ssize);		/* sample data */

	return 0;
}

static int test_fcm(const uint8 *data, char *t, int s)
{
	int j;

	PW_REQUEST_DATA(s, 37 + 8 * 31);

	/* "FC-M" : ID of FC-M packer */
	if (data[0] != 'F' || data[1] != 'C' || data[2] != '-' ||
		data[3] != 'M')
		return -1;

	/* test 1 */
	if (data[4] != 0x01)
		return -1;

	/* test 2 */
	if (data[5] != 0x00)
		return -1;

	/* test 3 */
	for (j = 0; j < 31; j++) {
		if (data[37 + 8 * j] > 0x40)
			return -1;
	}

	pw_read_title(data + 10, t, 20);
	return 0;
}

const struct pw_format pw_fcm = {
	"FC-M Packer",
	test_fcm,
	depack_fcm
};
