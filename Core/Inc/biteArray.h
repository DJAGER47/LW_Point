/*
 * biteArray.h
 *
 *  Created on: Feb 10, 2021
 *      Author: Admin
 */

#ifndef INC_BITEARRAY_H_
#define INC_BITEARRAY_H_

#include <stdint.h>

static uint8_t GetBit(uint8_t * x, uint16_t i){
	int k = i / 8;
	uint8_t b = (x[k] >> (i % 8)) & 0x01;
	return b;
}
static void SetBit(uint8_t * x, uint16_t i, bool b){
	if (GetBit(x, i) == b)
		return;
	char c = x[i / 8];
	x[i / 8] = c ^ (1 << (i % 8));  // Меняем нужный бит
}

#endif /* INC_BITEARRAY_H_ */
