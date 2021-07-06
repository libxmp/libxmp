#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_COMMENTS

#ifndef STB_VORBIS_C
/* client: */
#define STB_VORBIS_HEADER_ONLY
#include "vorbis.c"
#endif
