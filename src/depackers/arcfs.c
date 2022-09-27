/* Extended Module Player
 * Copyright (C) 2021-2022 Alice Rowan <petrifiedrowan@gmail.com>
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

/**
 * Simple single-file unpacker for ArcFS archives.
 * Report bugs to libxmp or to here: https://github.com/AliceLR/megazeuxtests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arc_unpack.h"
#include "depacker.h"
#include "crc32.h"

/* Arbitrary maximum allowed output filesize. */
#define ARCFS_MAX_OUTPUT LIBXMP_DEPACK_LIMIT

/* #define ARCFS_DEBUG */

#define ARCFS_HEADER_SIZE 96
#define ARCFS_ENTRY_SIZE 36

#define ARCFS_END_OF_DIR 0
#define ARCFS_DELETED 1

#ifdef ARCFS_DEBUG
#define debug(...) do{ fprintf(stderr, "" __VA_ARGS__); fflush(stderr); }while(0)
#endif

static arc_uint16 arc_crc16(arc_uint8 *buf, size_t len)
{
  return libxmp_crc16_IBM(buf, len, 0);
}

static arc_uint16 arc_mem_u16(arc_uint8 *buf)
{
  return (buf[1] << 8) | buf[0];
}

static arc_uint32 arc_mem_u32(arc_uint8 *buf)
{
  return (buf[3] << 24UL) | (buf[2] << 16UL) | (buf[1] << 8UL) | buf[0];
}

struct arcfs_data
{
  /*  0    char magic[8]; */
  /*  8 */ arc_uint32 entries_length;
  /* 12 */ arc_uint32 data_offset;
  /* 16 */ arc_uint32 min_read_version;
  /* 20 */ arc_uint32 min_write_version;
  /* 24 */ arc_uint32 format_version;
  /* 28    Filler. */
  /* 96 */
};

struct arcfs_entry
{
  /* Unhandled fields:
   * - Permissions are stored in the low byte of attributes.
   * - A 40-bit timestamp is usually stored in load_offset/exec_offset.
   *   The timestamp counts 10ms increments from epoch 1900-01-01.
   *   This is supposed to be stored when the top 12 bits of load_offset are 0xFFF.
   * - Likewise, when the top 12 bits of load_offset are 0xFF, bits 8 through 19
   *   in load_offset are supposed to be the RISC OS filetype.
   */

  /*  0 */ arc_uint8 method;
  /*  1 */ char filename[12];
  /* 12 */ arc_uint32 uncompressed_size;
  /* 16    arc_uint32 load_offset; */ /* Low byte -> high byte of the 40-bit timestamp. */
  /* 20    arc_uint32 exec_offset; */ /* Low portion of the 40-bit timestamp. */
  /* 24    arc_uint32 attributes; */
  /* 28 */ arc_uint32 compressed_size;
  /* 32    arc_uint32 info; */
  /* 36 */

  /* Unpacked fields */
  arc_uint16 crc16;
  arc_uint8  compression_bits;
  arc_uint8  is_directory;
  arc_uint32 value_offset;
};

static int arcfs_check_magic(const unsigned char *buf)
{
  return memcmp(buf, "Archive\x00", 8) ? -1 : 0;
}

static int arcfs_read_header(struct arcfs_data *data, HIO_HANDLE *f)
{
  arc_uint8 buffer[ARCFS_HEADER_SIZE];

  if(hio_read(buffer, 1, ARCFS_HEADER_SIZE, f) < ARCFS_HEADER_SIZE)
  {
    #ifdef ARCFS_DEBUG
    debug("short read in header\n");
    #endif
    return -1;
  }

  if(arcfs_check_magic(buffer) < 0)
  {
    #ifdef ARCFS_DEBUG
    debug("bad header magic: %8.8s\n", (char *)buffer);
    #endif
    return -1;
  }

  data->entries_length    = arc_mem_u32(buffer + 8);
  data->data_offset       = arc_mem_u32(buffer + 12);
  data->min_read_version  = arc_mem_u32(buffer + 16);
  data->min_write_version = arc_mem_u32(buffer + 20);
  data->format_version    = arc_mem_u32(buffer + 24);

  if(data->entries_length % ARCFS_ENTRY_SIZE != 0)
  {
    #ifdef ARCFS_DEBUG
    debug("bad entries length: %zu\n", (size_t)data->entries_length);
    #endif
    return -1;
  }

  if(data->data_offset < ARCFS_HEADER_SIZE ||
     data->data_offset - ARCFS_HEADER_SIZE < data->entries_length)
  {
    #ifdef ARCFS_DEBUG
    debug("bad data offset: %zu\n", (size_t)data->data_offset);
    #endif
    return -1;
  }

  /* These seem to be the highest versions that exist. */
  if(data->min_read_version > 260 || data->min_write_version > 260 || data->format_version > 0x0a)
  {
    #ifdef ARCFS_DEBUG
    debug("bad versions: %zu %zu %zu\n", (size_t)data->min_read_version,
     (size_t)data->min_write_version, (size_t)data->format_version);
    #endif
    return -1;
  }

  return 0;
}

static int arcfs_read_entry(struct arcfs_entry *e, HIO_HANDLE *f)
{
  arc_uint8 buffer[ARCFS_ENTRY_SIZE];

  if(hio_read(buffer, 1, ARCFS_ENTRY_SIZE, f) < ARCFS_ENTRY_SIZE)
    return -1;

  e->method = buffer[0] & 0x7f;
  if(e->method == ARCFS_END_OF_DIR)
    return 0;

  memcpy(e->filename, buffer + 1, 11);
  e->filename[11] = '\0';

  e->uncompressed_size = arc_mem_u32(buffer + 12);
  e->compression_bits  = buffer[25]; /* attributes */
  e->crc16             = arc_mem_u16(buffer + 26); /* attributes */
  e->compressed_size   = arc_mem_u32(buffer + 28);
  e->value_offset      = arc_mem_u32(buffer + 32) & 0x7fffffffUL; /* info */
  e->is_directory      = buffer[35] >> 7; /* info */

  return 0;
}

static int arcfs_read(unsigned char **dest, size_t *dest_len, HIO_HANDLE *f, unsigned long file_len)
{
  struct arcfs_data data;
  struct arcfs_entry e;
  unsigned char *in;
  unsigned char *out;
  const char *err;
  size_t out_len;
  size_t offset;
  size_t i;

  if(arcfs_read_header(&data, f) < 0)
    return -1;

  if(data.data_offset > file_len)
    return -1;

  for(i = 0; i < data.entries_length; i += ARCFS_ENTRY_SIZE)
  {
    if(arcfs_read_entry(&e, f) < 0)
    {
      #ifdef ARCFS_DEBUG
      debug("error reading entry %zu\n", (size_t)data.entries_length / ARCFS_ENTRY_SIZE);
      #endif
      return -1;
    }

    #ifdef ARCFS_DEBUG
    debug("checking file: %s\n", e.filename);
    #endif

    /* Ignore directories, end of directory markers, deleted files. */
    if(e.method == ARCFS_END_OF_DIR || e.method == ARCFS_DELETED || e.is_directory)
      continue;

    if(e.method == ARC_M_UNPACKED)
      e.compressed_size = e.uncompressed_size;

    /* Ignore junk offset/size. */
    if(e.value_offset >= file_len - data.data_offset)
      continue;

    offset = data.data_offset + e.value_offset;
    if(e.compressed_size > file_len - offset)
      continue;

    /* Ignore sizes over the allowed limit. */
    if(e.uncompressed_size > ARCFS_MAX_OUTPUT)
      continue;

    /* Ignore unsupported methods. */
    if(arc_method_is_supported(e.method) < 0)
      continue;

    if(libxmp_exclude_match(e.filename))
      continue;

    /* Read file. */
    #ifdef ARCFS_DEBUG
    debug("unpacking file: %s\n", e.filename);
    #endif

    if(hio_seek(f, offset, SEEK_SET) < 0)
      return -1;

    in = (unsigned char *)malloc(e.compressed_size);
    if(!in)
      return -1;

    if(hio_read(in, 1, e.compressed_size, f) < e.compressed_size)
    {
      free(in);
      return -1;
    }

    if(e.method != ARC_M_UNPACKED)
    {
      out = (unsigned char *)malloc(e.uncompressed_size);
      out_len = e.uncompressed_size;
      if(!out)
      {
        free(in);
        return -1;
      }

      err = arc_unpack(out, out_len, in, e.compressed_size, e.method, e.compression_bits);
      if(err != NULL)
      {
        #ifdef ARCFS_DEBUG
        debug("error unpacking: %s\n", err);
        #endif
        free(in);
        free(out);
        return -1;
      }
      free(in);
    }
    else
    {
      out = in;
      out_len = e.uncompressed_size;
    }

    /* ArcFS CRC may sometimes just be 0, in which case, ignore it. */
    if(e.crc16)
    {
      arc_uint16 out_crc16 = arc_crc16(out, out_len);
      if(e.crc16 != out_crc16)
      {
        #ifdef ARCFS_DEBUG
        debug("crc16 mismatch: expected %u, got %u\n", e.crc16, out_crc16);
        #endif
        free(out);
        return -1;
      }
    }
    *dest = out;
    *dest_len = out_len;
    return 0;
  }
  return -1;
}


static int arcfs_test(unsigned char *data)
{
  return arcfs_check_magic(data) == 0;
}

static int arcfs_decrunch(HIO_HANDLE *in, void **out, long *outlen)
{
  unsigned char *outbuf;
  size_t size;

  int ret = arcfs_read(&outbuf, &size, in, hio_size(in));
  if(ret < 0)
    return -1;

  *out = outbuf;
  *outlen = size;
  return 0;
}

const struct depacker libxmp_depacker_arcfs =
{
  arcfs_test,
  arcfs_decrunch
};
