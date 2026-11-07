#ifndef XMP_HIO_H
#define XMP_HIO_H

#include "callbackio.h"
#include "memio.h"

#define HIO_HANDLE_TYPE(x) ((x)->type)

enum hio_type {
	HIO_HANDLE_TYPE_FILE,
	HIO_HANDLE_TYPE_MEMORY,
	HIO_HANDLE_TYPE_CBFILE
};

#define HIO_BUFFER_SIZE 4096

/* includes a preview of next buffer */
#define HIO_FULL_BUFFER (HIO_BUFFER_SIZE + 4)

#define HIO_BUFFER_CURSOR(h) ((h)->cursor % HIO_BUFFER_SIZE)

typedef struct {
	uint8 buffer[HIO_FULL_BUFFER];
	long cursor;
	long size;
	union {
		FILE *file;
		MFILE *mem;
		CBFILE *cbfile;
	} handle;
	enum hio_type type;
	int buffer_end; /* proportion of the current buffer that is filled, relevant for EOF detection */
	int error;
	int noclose;
} HIO_HANDLE;

LIBXMP_BEGIN_DECLS

int8	hio_read8s	(HIO_HANDLE *);
uint8	hio_read8	(HIO_HANDLE *);
uint16	hio_read16l	(HIO_HANDLE *);
uint16	hio_read16b	(HIO_HANDLE *);
uint32	hio_read24l	(HIO_HANDLE *);
uint32	hio_read24b	(HIO_HANDLE *);
uint32	hio_read32l	(HIO_HANDLE *);
uint32	hio_read32b	(HIO_HANDLE *);
size_t	hio_read	(void *, size_t, size_t, HIO_HANDLE *);
int	hio_seek	(HIO_HANDLE *, long, int);
long	hio_tell	(HIO_HANDLE *);
int	hio_eof		(HIO_HANDLE *);
int	hio_error	(HIO_HANDLE *);
HIO_HANDLE *hio_open	(const char *, const char *);
HIO_HANDLE *hio_open_const_mem  (const void *, long);
HIO_HANDLE *hio_open_file (FILE *);
HIO_HANDLE *hio_open_file2 (FILE *);/* allows fclose()ing the file by libxmp */
HIO_HANDLE *hio_open_callbacks (void *, struct xmp_callbacks);
int	hio_reopen_mem	(void *, long, int, HIO_HANDLE *);
int	hio_reopen_file	(FILE *, int, HIO_HANDLE *);
int	hio_close	(HIO_HANDLE *);
long	hio_size	(HIO_HANDLE *);
const unsigned char *hio_get_underlying_memory(HIO_HANDLE *);

LIBXMP_END_DECLS

#endif
