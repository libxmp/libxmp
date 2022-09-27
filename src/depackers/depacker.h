#ifndef LIBXMP_DEPACKER_H
#define LIBXMP_DEPACKER_H

#include "../common.h"
#include "../hio.h"

extern const struct depacker libxmp_depacker_zip;
extern const struct depacker libxmp_depacker_lha;
extern const struct depacker libxmp_depacker_gzip;
extern const struct depacker libxmp_depacker_bzip2;
extern const struct depacker libxmp_depacker_xz;
extern const struct depacker libxmp_depacker_compress;
extern const struct depacker libxmp_depacker_pp;
extern const struct depacker libxmp_depacker_sqsh;
extern const struct depacker libxmp_depacker_arc;
extern const struct depacker libxmp_depacker_arcfs;
extern const struct depacker libxmp_depacker_mmcmp;
extern const struct depacker libxmp_depacker_lzx;
extern const struct depacker libxmp_depacker_s404;
extern const struct depacker libxmp_depacker_xfd;

struct depacker {
	int (*test)(unsigned char *);
	int (*depack)(HIO_HANDLE *, void **, long *);
};

int	libxmp_decrunch		(HIO_HANDLE *h, const char *filename, char **temp);
int	libxmp_exclude_match	(const char *);

#endif /* LIBXMP_DEPACKER_H */
