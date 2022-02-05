#ifndef LIBXMP_DEPACKER_H
#define LIBXMP_DEPACKER_H

#include "../common.h"
#include "../hio.h"

/* Output file size limit for files unpacked from unarchivers into RAM. Most
 * general archive compression formats can't nicely bound the output size
 * from their input filesize, and a cap is needed for a few reasons:
 *
 * - Linux is too dumb for its own good and its malloc/realloc will return
 *   pointers to RAM that doesn't exist instead of NULL. When these are used,
 *   it will kill the application instead of allowing it to fail gracefully.
 * - libFuzzer and the clang sanitizers have malloc/realloc interceptors that
 *   terminate with an error instead of returning NULL.
 *
 * Depackers that have better ways of bounding the output size can ignore this.
 * This value is fairly arbitrary and can be changed if needed.
 */
#define LIBXMP_DEPACK_LIMIT (512 << 20)

extern struct depacker libxmp_depacker_zip;
extern struct depacker libxmp_depacker_lha;
extern struct depacker libxmp_depacker_gzip;
extern struct depacker libxmp_depacker_bzip2;
extern struct depacker libxmp_depacker_xz;
extern struct depacker libxmp_depacker_compress;
extern struct depacker libxmp_depacker_pp;
extern struct depacker libxmp_depacker_sqsh;
extern struct depacker libxmp_depacker_arc;
extern struct depacker libxmp_depacker_arcfs;
extern struct depacker libxmp_depacker_mmcmp;
extern struct depacker libxmp_depacker_muse;
extern struct depacker libxmp_depacker_lzx;
extern struct depacker libxmp_depacker_s404;
extern struct depacker libxmp_depacker_xfd;

struct depacker {
	int (*test)(unsigned char *);
	int (*depack)(HIO_HANDLE *, FILE *, long);
	int (*depack_mem)(HIO_HANDLE *, void **, long, long *);
};

int	libxmp_decrunch		(HIO_HANDLE **h, const char *filename, char **temp);
int	libxmp_exclude_match	(const char *);

#endif /* LIBXMP_DEPACKER_H */
