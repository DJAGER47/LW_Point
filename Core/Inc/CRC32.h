/*
 * CRC32.h
 *
 *  Created on: 13 февр. 2020 г.
 *      Author: Admin
 */

#ifndef CRC32_H_
#define CRC32_H_

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

uint32_t crc_32(uint32_t crc, uint8_t *ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* CRC32_H_ */
