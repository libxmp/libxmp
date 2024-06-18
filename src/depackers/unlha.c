/* Extended Module Player
 * Copyright (C) 1996-2024 Claudio Matsuoka and Hipolito Carraro Jr
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

/* Changed to use lhasa library as backend by O.Sezer.  */

#include "depacker.h"
#include "lhasa/lhasa.h"

static int lhasa_read(void *handle, void *buf, size_t buf_len) {
    HIO_HANDLE *in = (HIO_HANDLE*)handle;
    size_t r = hio_read(buf, 1, buf_len, in);
    if (!r && hio_error(in)) {
        return -1;
    }
    return (int)r;
}

static int lhasa_skip(void *handle, size_t bytes) {
    return hio_seek((HIO_HANDLE*)handle, (long)bytes, SEEK_CUR) >= 0;
}

static const LHAInputStreamType io_callbacks = {
    lhasa_read,
    lhasa_skip,
    NULL
};

static int test_lha(unsigned char *b) {
    return b[2] == '-' && b[3] == 'l' && b[4] == 'h' && b[6] == '-' && b[20] <= 3;
}

static int decrunch_lha(HIO_HANDLE *in, void **out, long *outlen)
{
    LHAInputStream *stream;
    LHAReader *reader;
    LHAFileHeader *header;
    unsigned char *outbuf;
    int error = -1;

    stream = lha_input_stream_new(&io_callbacks, in);
    if (!stream) {
        return -1;
    }

    reader = lha_reader_new(stream);
    if (!reader) {
        lha_input_stream_free(stream);
        return -1;
    }

    for (;;) {
        header = lha_reader_next_file(reader);
        if (!header) {
            break;
        }
        if (!strcmp(header->compress_method, LHA_COMPRESS_TYPE_DIR)) {
            continue;   /* directory or symlink */
        }
        if (!libxmp_exclude_match(header->filename)) {
            break;
        }
    }

    if (!header || (long)header->length <= 0) {
        goto fail;
    }

    outbuf = (unsigned char *) malloc(header->length);
    if (!outbuf) {
        goto fail;
    }
    if (lha_reader_read(reader, outbuf, header->length) != header->length) {
        free(outbuf);
        goto fail;
    }

    *out = outbuf;
    *outlen = (long) header->length;
    error = 0;

fail:
    lha_reader_free(reader);
    lha_input_stream_free(stream);
    return error;
}

const struct depacker libxmp_depacker_lha = {
    test_lha,
    NULL,
    decrunch_lha
};
