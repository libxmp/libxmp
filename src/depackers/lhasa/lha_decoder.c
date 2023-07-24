/*

Copyright (c) 2011, 2012, Simon Howard

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

#include "../../common.h"
#include "../crc32.h"

#include "lha_decoder.h"


static struct {
	const char *name;
	LHADecoderType *dtype;
} decoders[] = {
	{ "-lz4-", &lha_null_decoder },
	{ "-lz5-", &lha_lz5_decoder },
	{ "-lzs-", &lha_lzs_decoder },
	{ "-lh0-", &lha_null_decoder },
	{ "-lh1-", &lha_lh1_decoder },
	{ "-lh4-", &lha_lh4_decoder },
	{ "-lh5-", &lha_lh5_decoder },
	{ "-lh6-", &lha_lh6_decoder },
	{ "-lh7-", &lha_lh7_decoder },
	{ "-lhx-", &lha_lhx_decoder },
	{ "-lk7-", &lha_lk7_decoder },
	{ "-pm0-", &lha_null_decoder },
	{ "-pm1-", &lha_pm1_decoder },
	{ "-pm2-", &lha_pm2_decoder },
};

LHADecoder *lha_decoder_new(LHADecoderType *dtype,
                            LHADecoderCallback callback,
                            void *callback_data,
                            size_t stream_length)
{
	LHADecoder *decoder;
	void *extra_data;

	// Space is allocated together: the LHADecoder structure,
	// then the private data area used by the algorithm,
	// followed by the output buffer,

	decoder = (LHADecoder *) calloc(1, sizeof(LHADecoder) + dtype->extra_size
	                        + dtype->max_read);

	if (decoder == NULL) {
		return NULL;
	}

	decoder->dtype = dtype;
	decoder->last_block = UINT_MAX;
	decoder->outbuf_pos = 0;
	decoder->outbuf_len = 0;
	decoder->stream_pos = 0;
	decoder->stream_length = stream_length;
	decoder->decoder_failed = 0;
	decoder->crc = 0;

	// Private data area follows the structure.

	extra_data = decoder + 1;
	decoder->outbuf = ((uint8 *) extra_data) + dtype->extra_size;

	if (dtype->init != NULL
	 && !dtype->init(extra_data, callback, callback_data)) {
		free(decoder);
		return NULL;
	}

	return decoder;
}

LHADecoderType *lha_decoder_for_name(const char *name)
{
	unsigned int i;

	for (i = 0; i < sizeof(decoders) / sizeof(*decoders); ++i) {
		if (!strcmp(name, decoders[i].name)) {
			return decoders[i].dtype;
		}
	}

	// Unknown?

	return NULL;
}

void lha_decoder_free(LHADecoder *decoder)
{
	if (decoder->dtype->free != NULL) {
		decoder->dtype->free(decoder + 1);
	}

	free(decoder);
}

size_t lha_decoder_read(LHADecoder *decoder, uint8 *buf, size_t buf_len)
{
	size_t filled, bytes;

	// When we reach the end of the stream, we must truncate the
	// decompressed data at exactly the right point (stream_length),
	// or we may read a few extra false byte(s) by mistake.
	// Reduce buf_len when we get to the end to limit it to the
	// real number of remaining characters.

	if (decoder->stream_pos + buf_len > decoder->stream_length) {
		buf_len = decoder->stream_length - decoder->stream_pos;
	}

	// Try to fill up the buffer that has been passed with as much
	// data as possible. Each call to read() will fill up outbuf
	// with some data; this is then copied into buf, with some
	// data left at the end for the next call.

	filled = 0;

	while (filled < buf_len) {

		// Try to empty out some of the output buffer first.

		bytes = decoder->outbuf_len - decoder->outbuf_pos;

		if (buf_len - filled < bytes) {
			bytes = buf_len - filled;
		}

		memcpy(buf + filled, decoder->outbuf + decoder->outbuf_pos,
		       bytes);
		decoder->outbuf_pos += bytes;
		filled += bytes;

		// If we previously encountered a failure reading from
		// the decoder, don't try to call the read function again.

		if (decoder->decoder_failed) {
			break;
		}

		// If outbuf is now empty, we can process another run to
		// re-fill it.

		if (decoder->outbuf_pos >= decoder->outbuf_len) {
			decoder->outbuf_len
			    = decoder->dtype->read(decoder + 1,
			                           decoder->outbuf);
			decoder->outbuf_pos = 0;
		}

		// No more data to be read?

		if (decoder->outbuf_len == 0) {
			decoder->decoder_failed = 1;
			break;
		}
	}

	// Update CRC.

	decoder->crc = libxmp_crc16_IBM(buf, filled, decoder->crc);

	// Track stream position.

	decoder->stream_pos += filled;

	return filled;
}
