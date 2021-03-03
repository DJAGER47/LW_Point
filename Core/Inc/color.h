/*
 * color.h
 *
 *  Created on: Feb 11, 2021
 *      Author: Admin
 */

#ifndef INC_COLOR_H_
#define INC_COLOR_H_


#define HUE(h)  ({ typeof(h) h1 = h % 360; h1 < 0 ? 360 + h1 : h1; })
class COLOR{
public:
	typedef struct {
		uint8_t r;       // a fraction between 0 and 1
		uint8_t g;       // a fraction between 0 and 1
		uint8_t b;       // a fraction between 0 and 1
	} _rgb;

	typedef struct {
		uint16_t h;      // Hue Ч цветовой тон
		uint8_t s;       // Saturation Ч насыщенность
		uint8_t v;       // Value (значение цвета) или Brightness Ч €ркость
	} _hsv;

	static _rgb hsv2rgb(const _hsv &hsv) {
		_rgb rgb = { 0 };
		if (!hsv.v) {
			rgb.r = rgb.g = rgb.b = 0;
		} else if (!hsv.s) {
			rgb.r = rgb.g = rgb.b = hsv.v;
		} else {
			auto hue = HUE(hsv.h);

			auto sector = hue / 60;
			auto angle = sector & 1 ? 60 - hue % 60 : hue % 60;

			auto high = hsv.v;
			auto low = (255 - hsv.s) * high / 255;
			auto middle = low + (high - low) * angle / 60;

			switch (sector) {
			case 0: // red . yellow
				rgb.r = high;
				rgb.g = middle;
				rgb.b = low;
				break;

			case 1: // yellow . green
				rgb.r = middle;
				rgb.g = high;
				rgb.b = low;
				break;

			case 2: // green . cyan
				rgb.r = low;
				rgb.g = high;
				rgb.b = middle;
				break;

			case 3: // cyan . blue
				rgb.r = low;
				rgb.g = middle;
				rgb.b = high;
				break;

			case 4: // blue . magenta
				rgb.r = middle;
				rgb.g = low;
				rgb.b = high;
				break;

			case 5: // magenta . red
				rgb.r = high;
				rgb.g = low;
				rgb.b = middle;
			}
		}
		return rgb;
	}


	/*_hsv rgb2hsv(const _rgb &in) {
		_hsv out;
		double min, max, delta;

		min = in.r < in.g ? in.r : in.g;
		min = min < in.b ? min : in.b;

		max = in.r > in.g ? in.r : in.g;
		max = max > in.b ? max : in.b;

		out.v = max; // v
		delta = max - min;
		if (delta < 0.00001) {
			out.s = 0;
			out.h = 0; // undefined, maybe nan?
			return out;
		}
		if (max > 0) { // NOTE: if Max is == 0, this divide would cause a crash
			out.s = (delta / max);                  // s
		} else {
			// if max is 0, then r = g = b = 0
			// s = 0, h is undefined
			out.s = 0;
			out.h = 0;                            // its now undefined
			return out;
		}
		if (in.r >= max)                    // > is bogus, just keeps compilor happy
			out.h = (in.g - in.b) / delta;        // between yellow & magenta
		else if (in.g >= max)
			out.h = 2 + (in.b - in.r) / delta;  // between cyan & yellow
		else
			out.h = 4 + (in.r - in.g) / delta;  // between magenta & cyan

		out.h *= 60;                              // degrees

		if (out.h < 0)
			out.h += 360;

		return out;
	}*/
};









#endif /* INC_COLOR_H_ */
