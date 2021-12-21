#ifndef LIBXMP_INFLATE_H
#define LIBXMP_INFLATE_H

#include "../hio.h"

struct inflate_data {
	struct huffman_tree_t *huffman_tree_len_static;
};

int	libxmp_inflate	(HIO_HANDLE *, FILE *, uint32 *, int);

#endif
