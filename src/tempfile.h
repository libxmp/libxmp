#ifndef XMP_PLATFORM_H
#define XMP_PLATFORM_H

#include "common.h"

LIBXMP_BEGIN_DECLS

FILE *make_temp_file(char **);
void unlink_temp_file(char *);

LIBXMP_END_DECLS

#endif
