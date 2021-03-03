/*
 * RGBRing.h
 *
 *  Created on: Feb 10, 2021
 *      Author: Admin
 */

#ifndef RGBRING_RGBRING_H_
#define RGBRING_RGBRING_H_

#include <stdint.h>
#include "color.h"

class RGBRing {
public:
	const uint8_t countLed;
	uint64_t led;
public:
	RGBRing();
	void init();
	void writeLed(uint8_t num, bool enable);
	void update();

	void setColor(COLOR::_rgb& in);
	void setColor(COLOR::_hsv& in);
};

#endif /* RGBRING_RGBRING_H_ */
