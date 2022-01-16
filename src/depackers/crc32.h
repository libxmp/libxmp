#ifndef LIBXMP_CRC_H
#define LIBXMP_CRC_H

#include "../common.h"

LIBXMP_BEGIN_DECLS

uint32	libxmp_crc32_A		(const uint8 *, size_t, uint32);
uint32	libxmp_crc32_A_no_inv	(const uint8 *, size_t, uint32);
uint16	libxmp_crc16_IBM	(const uint8 *, size_t, uint16);

/* TODO remove or replace: */
extern uint32 libxmp_crc32_table_B[256];
void	libxmp_crc32_init_B	(void);

LIBXMP_END_DECLS

#endif

