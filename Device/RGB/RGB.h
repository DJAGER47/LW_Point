/*
 * RGB.h
 *
 *  Created on: 5 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef RGB_RGB_H_
#define RGB_RGB_H_

#include <stdint.h>
#include "color.h"

class RGB {
public:
	RGB();
	void init();
	void setColor(COLOR::_rgb &in);
	void setColor(COLOR::_hsv& in);
};

#endif /* RGB_RGB_H_ */
