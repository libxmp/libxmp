#ifndef LIBXMP_TEMPFILE_H
#define LIBXMP_TEMPFILE_H

#include "common.h"

LIBXMP_BEGIN_DECLS

FILE *make_temp_file(char **);
void unlink_temp_file(char *);

#ifndef HAVE_MKSTEMP
int libxmp_mkstemp(char *);
#define mkstemp libxmp_mkstemp
#endif

LIBXMP_END_DECLS

#endif
