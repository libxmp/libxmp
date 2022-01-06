#ifndef LIBXMP_CRC_H
#define LIBXMP_CRC_H

#include "../common.h"

LIBXMP_BEGIN_DECLS

uint32	libxmp_crc32_A		(const uint8 *, size_t, uint32);
uint32	libxmp_crc32_A_no_inv	(const uint8 *, size_t, uint32);
uint16	libxmp_crc16_IBM	(const uint8 *, size_t, uint16);

LIBXMP_END_DECLS

#endif

