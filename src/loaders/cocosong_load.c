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

/* CoconizerSong Relocatable Module player with embedded Coconizer module.
 *
 * These seem to be about as common as Coconizer modules,
 * which is the only reason for implementing support for them.
 * Unfortunately, doing this reliably involves parsing ARM instructions.
 *
 * These modules also sometimes have copyrights/credits and extended author
 * comments that are not mirrored inside of the Coconizer format.
 */

#include "loader.h"

static int cocosong_test (HIO_HANDLE *, char *, const int);
static int cocosong_load (struct module_data *, HIO_HANDLE *, const int);

static int cocosong_start_position(HIO_HANDLE *, const int, const int);

const struct format_loader libxmp_loader_cocosong = {
	"CoconizerSong",
	cocosong_test,
	cocosong_load
};

static int cocosong_test(HIO_HANDLE *f, char *t, const int start)
{
	uint8 buf[44];
	uint32 start_address;
	uint32 init_address;
	uint32 finish_address;
	uint32 service_handler;
	uint32 title_address;
	uint32 help_address;
	uint32 keywords_address;
	int pos;

	if (hio_read(buf, 1, 44, f) < 44)
		return -1;

	/* Acorn 26-bit relocatable module format. */
	start_address = readmem32l(buf + 0);
	init_address = readmem32l(buf + 4);
	finish_address = readmem32l(buf + 8);
	service_handler = readmem32l(buf + 12);
	title_address = readmem32l(buf + 16);
	help_address = readmem32l(buf + 20);
	keywords_address = readmem32l(buf + 24);

	if (start_address != 0 ||
	    (init_address & 3) || init_address < 0x2c ||
	    init_address >= 0x400 ||
	    (finish_address & 3) || finish_address < 0x2c ||
	    finish_address >= 0x400 ||
	    finish_address < init_address ||
	    service_handler != 0 ||
	    title_address != 28 ||
	    (help_address & 3) || help_address >= 0x400 ||
	    (help_address && help_address < 0x2c) ||
	    help_address > init_address ||
	    (keywords_address & 3) || keywords_address >= 0x400 ||
	    (keywords_address && keywords_address < 0x2c) ||
	    keywords_address > init_address ||
	    (help_address && keywords_address &&
	     help_address > keywords_address)) {
		return -1;
	}

	if (memcmp(buf + 28, "CoconizerSong\0\0\0", 16))
		return -1;

	pos = cocosong_start_position(f, (int)finish_address, start);
	if (pos < 0)
		return -1;

	hio_seek(f, start + pos + 1, SEEK_SET);
	libxmp_read_title(f, t, 20);
	return 0;
}


/* CoconizerSong executables don't contain a convenient module address.
 * They use two instances of ADR (10,Track) to source the track address.
 * ADR will emit either ADD or SUB instructions; in this case, it should
 * almost always be two ADD instructions.
 *
 * From finish address, load 1024 and scan for the instruction:
 *   31[cond]28 27[00]26 [immediate if 1]25 24[opcode]21 [status]20
 *   19[Rn]16 15[Rd]12 11[operand2]0
 *
 * [1110=always][00][1][0100=ADD][0]
 * [Rn=PC=1111][Rd=R10=1010][PC-relative offset]
 * xx Ax 8F E2
 *
 * Example: Computer Festival 1 by Neil Coffey
 * Module is at 0xb98. This particular module has two usable instances:
 *
 * PC = 0x2c4 (pipelining)
 * 2bc: e28fab02	-> ADD R10, PC, (2 << 10)
 * 2c0: e28aa0d4	-> ADD R10, R10, 0x0d4
 * 			-> R10 = 0xB98
 *
 * PC = 0x300 (pipelining)
 * 2f8: e28fab02	-> ADD R10, PC, (2 << 10)
 * 2fc: e28aa098	-> ADD R10, R10, 0x098
 * 			-> R10 = 0xB98
 */
#define ADR_INSTR(x)		((x) & 0xfffff000ul)
#define ADR_ADD_R10_PC		0xe28fa000ul
#define ADR_ADD_R10_R10		0xe28aa000ul
#define ADR_IMM_SHIFT(x)	(((x) & 0xf00u) >> 7u)
#define ADR_IMM_BASE(x)		((x) & 0xffu)

static unsigned cocosong_get_imm(uint32 instruction)
{
	uint32 value = ADR_IMM_BASE(instruction);
	uint32 r = ADR_IMM_SHIFT(instruction);
	uint32 l = 32u - r;

	return (r == 0) ? value : (value >> r) | (value << l);
}

static int cocosong_start_position(HIO_HANDLE *f, const int finish_address,
				   const int start)
{
	uint8 buf[1024];
	uint8 *pos;
	uint8 *end;
	uint32 instruction;
	int offset;
	int pc;
	uint8 x;

	if (hio_seek(f, start + finish_address, SEEK_SET) < 0)
		return -1;
	if (hio_read(buf, 1, sizeof(buf), f) < sizeof(buf))
		return -1;

	pos = buf;
	end = pos + sizeof(buf);
	pc = start + finish_address;
	while (pos < end) {
		instruction = readmem32l(pos);
		pos += 4;
		pc += 4;
		if (ADR_INSTR(instruction) != ADR_ADD_R10_PC)
			continue;

		/* PC + 8 (pipelining) - 4 (pre-incremented above) */
		offset = pc + 4;
		offset += cocosong_get_imm(instruction);

		/* Most likely two ADD instructions required, check the next. */
		if (pos < end) {
			instruction = readmem32l(pos);
			pos += 4;
			pc += 4;
			if (ADR_INSTR(instruction) == ADR_ADD_R10_R10)
				offset += cocosong_get_imm(instruction);
		}

		if (hio_seek(f, start + offset, SEEK_SET) < 0)
			continue;

		/* Offset should contain the initial channel
		 * count byte without the module flag set. */
		x = hio_read8(f);
		if (x != 0x04 && x != 0x08)
			continue;

		return offset;
	}
	return -1;
}

/* Coconizer's executables use the help and keyword table
 * messages to store author comments. */
static void cocosong_load_comments(struct module_data *m, HIO_HANDLE *f,
	const int start, int init_address, int finish_address,
	int help_address, int keyword_address)
{
	uint8 buf[32];
	int help_size = 0;
	int keyword_size = 0;
	int size = 0;
	int i;

	if (help_address) {
		if (keyword_address)
			help_size = MIN(keyword_address - help_address, 36);
		else
			help_size = MIN(init_address - help_address, 36);
	}
	if (keyword_address) {
		keyword_size = MIN(init_address - keyword_address, 1024);
		/* 32-byte "CocoInfo" header */
		if (keyword_size <= 32)
			keyword_size = 0;
	}

	m->comment = (char *) malloc(help_size + 1 + keyword_size + 1);
	if (m->comment == NULL)
		return;

	if (help_size && hio_seek(f, start + help_address, SEEK_SET) == 0) {
		D_(D_INFO "help message @ %d:%d", help_address, help_size);
		help_size = hio_read(m->comment, 1, help_size, f);
		m->comment[help_size] = '\n';
		size = help_size + 1;
	}

	if (keyword_size && hio_seek(f, start + keyword_address, SEEK_SET) == 0) {
		memset(buf, 0, sizeof(buf));
		hio_read(buf, 1, 32, f);

		if (!memcmp(buf + 0, "CocoInfo", 8) &&
		    readmem32l(buf + 8) == 0 &&
		    readmem32l(buf + 12) == 0 &&
		    readmem32l(buf + 16) == 0 &&
		    readmem32l(buf + 20) == 0 &&
		    readmem32l(buf + 24) == keyword_address + 32 &&
		    readmem32l(buf + 28) == 0) {
			D_(D_INFO "CocoInfo @ %d:%d", keyword_address, keyword_size);
			keyword_size = hio_read(m->comment + size,
				1, keyword_size - 32, f);
			size += keyword_size;
		}
	}
	m->comment[size] = '\0';

	for (i = 0; i < size; i++) {
		uint8 ch = m->comment[i];

		if ((ch < 32 && ch != '\n' && ch != '\t') || ch > 127)
			m->comment[i] = ' ';
	}
}

static int cocosong_load(struct module_data *m, HIO_HANDLE *f, const int start)
{
	uint8 buf[44];
	char title[32];
	int init_address;
	int finish_address;
	int help_address;
	int keyword_address;
	int pos;
	int ret;

	if (hio_read(buf, 1, 44, f) < 44)
		return -1;

	init_address = readmem32l(buf + 4);
	finish_address = readmem32l(buf + 8);
	help_address = readmem32l(buf + 20);
	keyword_address = readmem32l(buf + 24);

	pos = cocosong_start_position(f, finish_address, start);
	D_(D_INFO "song start position: %08x", pos);
	if (pos < 0)
		return -1; /* should be unreachable */

	/* Run the test since it currently contains essential sanity checks. */
	hio_seek(f, start + pos, SEEK_SET);
	if (libxmp_loader_coco.test(f, title, start + pos) < 0) {
		D_(D_CRIT "failed Coconizer header/instrument checks");
		return -1;
	}

	hio_seek(f, start + pos, SEEK_SET);
	ret = libxmp_loader_coco.loader(m, f, start + pos);
	if (ret < 0)
		return ret;

	if (!m->comment && help_address > 0)
		cocosong_load_comments(m, f, start, init_address,
			finish_address, help_address, keyword_address);

	return 0;
}
