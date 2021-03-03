/*
 * ws2812.h
 *
 *  Created on: Jul 26, 2020
 *      Author: Admin
 */

#ifndef DEVICE_WS2812_WS2812B_H_
#define DEVICE_WS2812_WS2812B_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define LED_COUNT 10 // >= 3
//--------------------------------------------------
#define DELAY_LEN 50
#define ARRAY_LEN_BUF (LED_COUNT*24)

#define WS2812_HIGH 58
#define WS2812_LOW  29

struct RGB{
    uint8_t r, g, b;
};

struct HSV{
    int16_t h;
    uint8_t s, v;
};



typedef struct RGB RGB_t;
typedef struct HSV HSV_t;
#define HUE(h)  ({ typeof(h) h1 = h % 360; h1 < 0 ? 360 + h1 : h1; })

void HSV2RGB(HSV_t *hsv, RGB_t *rgb);

class WS2812B {

	enum LED_COLOR{
		_rgb,
		_hsv
	};

	TIM_HandleTypeDef *htim;
	uint32_t Channel;
	uint16_t countLed;

	uint16_t countSend;
	uint8_t endTransfer;
	uint8_t numBuf;
	uint8_t buf[2][ARRAY_LEN_BUF];

	void *_ledBuf;
	LED_COLOR _ledColor;

	void RGB2PWM(uint8_t* buf, RGB_t *rgb, uint8_t numLed);
	void HSV2PWM(uint8_t* buf, HSV_t *hsv, uint8_t numLed);
	void toBuf();
public:
	WS2812B(TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t countLed);
	void start();
	void UpdateBufferFULL();
	void UpdateBufferHALF();

	void Update(RGB_t *rgb);
	void Update(HSV_t *hsv);
};

#endif /* DEVICE_WS2812_WS2812B_H_ */
