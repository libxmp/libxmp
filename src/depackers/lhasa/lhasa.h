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

#ifndef LHASA_PUBLIC_LHASA_H
#define LHASA_PUBLIC_LHASA_H

#include "../../common.h"

/* uglify global functions */
#define lha_input_stream_new libxmp_lha_input_stream_new
#define lha_input_stream_free libxmp_lha_input_stream_free
#define lha_reader_new libxmp_lha_reader_new
#define lha_reader_free libxmp_lha_reader_free
#define lha_reader_next_file libxmp_lha_reader_next_file
#define lha_reader_read libxmp_lha_reader_read
#define lha_reader_current_is_fake libxmp_lha_reader_current_is_fake

#ifdef __cplusplus
extern "C" {
#endif

/**
 * lha_file_header.h
 *
 * LHA file header structure.
 *
 * This file contains the definition of the @ref LHAFileHeader structure,
 * representing a decoded file header from an LZH file.
 */

/** OS type value for an unknown OS. */
#define LHA_OS_TYPE_UNKNOWN            0x00
/** OS type value for Microsoft MS/DOS. */
#define LHA_OS_TYPE_MSDOS              'M'
/** OS type value for Microsoft Windows 95. */
#define LHA_OS_TYPE_WIN95              'w'
/** OS type value for Microsoft Windows NT. */
#define LHA_OS_TYPE_WINNT              'W'
/** OS type value for Unix. */
#define LHA_OS_TYPE_UNIX               'U'
/** OS type value for IBM OS/2. */
#define LHA_OS_TYPE_OS2                '2'
/** OS type for Apple Mac OS (Classic). */
#define LHA_OS_TYPE_MACOS              'm'
/** OS type for Amiga OS. */
#define LHA_OS_TYPE_AMIGA              'A'
/** OS type for Atari TOS. */
#define LHA_OS_TYPE_ATARI              'a'

// Obscure:

/** OS type for Sun (Oracle) Java. */
#define LHA_OS_TYPE_JAVA               'J'
/** OS type for Digital Research CP/M. */
#define LHA_OS_TYPE_CPM                'C'
/** OS type for Digital Research FlexOS. */
#define LHA_OS_TYPE_FLEX               'F'
/** OS type for Runser (?). */
#define LHA_OS_TYPE_RUNSER             'R'
/** OS type for Fujitsu FM Towns OS. */
#define LHA_OS_TYPE_TOWNSOS            'T'
/** OS type for Microware OS-9. */
#define LHA_OS_TYPE_OS9                '9'
/** OS type for Microware OS-9/68k. */
#define LHA_OS_TYPE_OS9_68K            'K'
/** OS type for OS/386 (?). */
#define LHA_OS_TYPE_OS386              '3'
/** OS type for Sharp X68000 Human68K OS. */
#define LHA_OS_TYPE_HUMAN68K           'H'
/** "OS type" that is used by the LHARK tool and does not indicate an
    OS as such, except that LHARK only runs under DOS. */
#define LHA_OS_TYPE_LHARK              ' '

/**
 * Compression type for a stored directory. The same value is also
 * used for Unix symbolic links.
 */
#define LHA_COMPRESS_TYPE_DIR   "-lhd-"

/**
 * Bit field value set in extra_flags to indicate that the
 * Unix file permission header (0x50) was parsed.
 */
#define LHA_FILE_UNIX_PERMS            0x01

/**
 * Bit field value set in extra_flags to indicate that the
 * Unix UID/GID header (0x51) was parsed.
 */
#define LHA_FILE_UNIX_UID_GID          0x02

/**
 * Bit field value set in extra_flags to indicate that the 'common
 * header' extended header (0x00) was parsed, and the common_crc
 * field has been set.
 */
#define LHA_FILE_COMMON_CRC            0x04

/**
 * Bit field value set in extra_flags to indicate that the
 * Windows time stamp header (0x41) was parsed, and the Windows
 * FILETIME timestamp fields have been set.
 */
#define LHA_FILE_WINDOWS_TIMESTAMPS    0x08

/**
 * Bit field value set in extra_flags to indicate that the OS-9
 * permissions field is set.
 */
#define LHA_FILE_OS9_PERMS             0x10

typedef struct _LHAFileHeader LHAFileHeader;

#define LHA_FILE_HAVE_EXTRA(header, flag) \
	(((header)->extra_flags & (flag)) != 0)
/**
 * Structure containing a decoded LZH file header.
 *
 * A file header precedes the compressed data of each file stored
 * within an LZH archive. It contains the name of the file, and
 * various additional metadata, some of which is optional, and
 * can depend on the header format, the tool used to create the
 * archive, and the operating system on which it was created.
 */

struct _LHAFileHeader {

	// Internal fields, do not touch!

	unsigned int _refcount;
	LHAFileHeader *_next;

	/**
	 * Stored path, with Unix-style ('/') path separators.
	 *
	 * This may be NULL, although if this is a directory
	 * (@ref LHA_COMPRESS_TYPE_DIR), it is never NULL.
	 */
	char *path;

	/**
	 * File name.
	 *
	 * This is never NULL, except if this is a directory
	 * (@ref LHA_COMPRESS_TYPE_DIR), where it is always NULL.
	 */
	char *filename;

	/**
	 * Compression method.
	 *
	 * If the header represents a directory or a symbolic link, the
	 * compression method is equal to @ref LHA_COMPRESS_TYPE_DIR.
	 */
	char compress_method[6];

	/** Length of the compressed data. */
	size_t compressed_length;

	/** Length of the uncompressed data. */
	size_t length;

	/** LZH header format used to store this header. */
	uint8 header_level;

	/**
	 * OS type indicator, identifying the OS on which
	 * the archive was created.
	 */
	uint8 os_type;

	/** 16-bit CRC of the compressed data. */
	uint16 crc;

	/** Unix timestamp of the modification time of the file. */
	unsigned int timestamp;

	/** Pointer to a buffer containing the raw header data. */
	uint8 *raw_data;

	/** Length of the raw header data. */
	size_t raw_data_len;

	/**
	 * Flags bitfield identifying extra data decoded from extended
	 * headers.
	 */
	unsigned int extra_flags;

	/** 16-bit CRC of header contents. */
	uint16 common_crc;
};


/**
 * lha_input_stream.h
 *
 * LHA input stream structure.
 *
 * This file defines the functions relating to the @ref LHAInputStream
 * structure, used to read data from an LZH file.
 */

/**
 * Opaque structure, representing an input stream used to read data from
 * an LZH file.
 */

typedef struct _LHAInputStream LHAInputStream;

/**
 * Structure containing pointers to callback functions to read data from
 * the input stream.
 */

typedef struct {

	/**
	 * Read a block of data into the specified buffer.
	 *
	 * @param handle       Handle pointer.
	 * @param buf          Pointer to buffer in which to store read data.
	 * @param buf_len      Size of buffer, in bytes.
	 * @return             Number of bytes read, or -1 for error.
	 */

	int (*read)(void *handle, void *buf, size_t buf_len);


	/**
	 * Skip the specified number of bytes from the input stream.
	 * This is an optional function.
	 *
	 * @param handle       Handle pointer.
	 * @param bytes        Number of bytes to skip.
	 * @return             Non-zero for success, or zero for failure.
	 */

	int (*skip)(void *handle, size_t bytes);

	/**
	 * Close the input stream.
	 *
	 * @param handle       Handle pointer.
	 */

	void (*close)(void *handle);

} LHAInputStreamType;

/**
 * Create new @ref LHAInputStream structure, using a set of generic functions
 * to provide LHA data.
 *
 * @param type         Pointer to a @ref LHAInputStreamType structure
 *                     containing callback functions to read data.
 * @param handle       Handle pointer to be passed to callback functions.
 * @return             Pointer to a new @ref LHAInputStream or NULL for error.
 */

LHAInputStream *lha_input_stream_new(const LHAInputStreamType *type,
                                     void *handle);

/**
 * Free an @ref LHAInputStream structure.
 *
 * @param stream       The input stream.
 */

void lha_input_stream_free(LHAInputStream *stream);


/**
 * lha_reader.h
 *
 * LHA file reader.
 *
 * This file contains the interface functions for the @ref LHAReader
 * structure, used to decode data from a compressed LZH file and
 * extract compressed files.
 */

/**
 * Opaque structure used to decode the contents of an LZH file.
 */

typedef struct _LHAReader LHAReader;

/**
 * Policy for extracting directories.
 *
 * When extracting a directory, some of the metadata associated with
 * it needs to be set after the contents of the directory have been
 * extracted. This includes the modification time (which would
 * otherwise be reset to the current time) and the permissions (which
 * can affect the ability to extract files into the directory).
 * To work around this problem there are several ways of handling
 * directory extraction.
 */

typedef enum {

	/**
	 * "Plain" policy. In this mode, the metadata is set at the
	 * same time that the directory is created. This is the
	 * simplest to comprehend, and the files returned from
	 * @ref lha_reader_next_file will match the files in the
	 * archive, but it is not recommended.
	 */

	LHA_READER_DIR_PLAIN,

	/**
	 * "End of directory" policy. In this mode, if a directory
	 * is extracted, the directory name will be saved. Once the
	 * contents of the directory appear to have been extracted
	 * (i.e. a file is found that is not within the directory),
	 * the directory will be returned again by
	 * @ref lha_reader_next_file. This time, when the directory
	 * is "extracted" (via @ref lha_reader_extract), the metadata
	 * will be set.
	 *
	 * This method uses less memory than
	 * @ref LHA_READER_DIR_END_OF_FILE, but there is the risk
	 * that a file will appear within the archive after the
	 * metadata has been set for the directory. However, this is
	 * not normally the case, as files and directories typically
	 * appear within an archive in order. GNU tar uses the same
	 * method to address this problem with tar files.
	 *
	 * This is the default policy.
	 */

	LHA_READER_DIR_END_OF_DIR,

	/**
	 * "End of file" policy. In this mode, each directory that
	 * is extracted is recorded in a list. When the end of the
	 * archive is reached, these directories are returned again by
	 * @ref lha_reader_next_file. When the directories are
	 * "extracted" again (via @ref lha_reader_extract), the
	 * metadata is set.
	 *
	 * This avoids the problems that can potentially occur with
	 * @ref LHA_READER_DIR_END_OF_DIR, but uses more memory.
	 */

	LHA_READER_DIR_END_OF_FILE

} LHAReaderDirPolicy;

/**
 * Create a new @ref LHAReader to read data from an @ref LHAInputStream.
 *
 * @param stream     The input stream to read data from.
 * @return           Pointer to a new @ref LHAReader structure,
 *                   or NULL for error.
 */

LHAReader *lha_reader_new(LHAInputStream *stream);

/**
 * Free a @ref LHAReader structure.
 *
 * @param reader     The @ref LHAReader structure.
 */

void lha_reader_free(LHAReader *reader);

/**
 * Read the header of the next archived file from the input stream.
 *
 * @param reader     The @ref LHAReader structure.
 * @return           Pointer to an @ref LHAFileHeader structure, or NULL if
 *                   an error occurred.  This pointer is only valid until
 *                   the next time that lha_reader_next_file is called.
 */

LHAFileHeader *lha_reader_next_file(LHAReader *reader);

/**
 * Read some of the (decompresed) data for the current archived file,
 * decompressing as appropriate.
 *
 * @param reader     The @ref LHAReader structure.
 * @param buf        Pointer to a buffer in which to store the data.
 * @param buf_len    Size of the buffer, in bytes.
 * @return           Number of bytes stored in the buffer, or zero if
 *                   there is no more data to decompress.
 */

size_t lha_reader_read(LHAReader *reader, void *buf, size_t buf_len);

/**
 * Check if the current file (last returned by @ref lha_reader_next_file)
 * was generated internally by the extract process. This occurs when a
 * directory or symbolic link must be created as a two-stage process, with
 * some of the extraction process deferred to later in the stream.
 *
 * These "fake" duplicates should usually be hidden in the user interface
 * when a summary of extraction is presented.
 *
 * @param reader         The @ref LHAReader structure.
 * @return               Non-zero if the current file is a "fake", or zero
 *                       for a normal file.
 */

int lha_reader_current_is_fake(LHAReader *reader);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef LHASA_PUBLIC_LHASA_H */
