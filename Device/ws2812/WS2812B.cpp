/*
 * ws2812.cpp
 *
 *  Created on: Jul 26, 2020
 *      Author: Admin
 */

#include "WS2812B.h"

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void HSV2RGB(HSV_t *hsv, RGB_t *rgb) {
	if (!hsv->v) {
		rgb->r = rgb->g = rgb->b = 0;
	} else if (!hsv->s) {
		rgb->r = rgb->g = rgb->b = hsv->v;
	} else {
		int hue = HUE(hsv->h);

		int sector = hue / 60;
		int angle = sector & 1 ? 60 - hue % 60 : hue % 60;

		int high = hsv->v;
		int low = (255 - hsv->s) * high / 255;
		int middle = low + (high - low) * angle / 60;

		switch (sector) {
		case 0: // red -> yellow
			rgb->r = high;
			rgb->g = middle;
			rgb->b = low;
			break;

		case 1: // yellow -> green
			rgb->r = middle;
			rgb->g = high;
			rgb->b = low;
			break;

		case 2: // green -> cyan
			rgb->r = low;
			rgb->g = high;
			rgb->b = middle;
			break;

		case 3: // cyan -> blue
			rgb->r = low;
			rgb->g = middle;
			rgb->b = high;
			break;

		case 4: // blue -> magenta
			rgb->r = middle;
			rgb->g = low;
			rgb->b = high;
			break;

		case 5: // magenta -> red
			rgb->r = high;
			rgb->g = low;
			rgb->b = middle;
		}
	}
}


void WS2812B::RGB2PWM(uint8_t* buf, RGB_t *rgb, uint8_t numLed){
    uint8_t mask = 0x80;
    for (uint8_t i = 0; i < 8; i++){
    	buf[24*numLed + 0 + i] = rgb->g & mask ? WS2812_HIGH : WS2812_LOW;
    	buf[24*numLed + 8 + i] = rgb->r & mask ? WS2812_HIGH : WS2812_LOW;
    	buf[24*numLed +16 + i] = rgb->b & mask ? WS2812_HIGH : WS2812_LOW;
        mask >>= 1;
    }
}

void WS2812B::HSV2PWM(uint8_t* buf, HSV_t *hsv, uint8_t numLed){
	RGB_t rgb;
	HSV2RGB(hsv, &rgb);
	RGB2PWM(buf, &rgb, numLed);
}


void WS2812B::toBuf(){

	if (!_ledBuf) return;

	if (!countSend){
		endTransfer = 2;
		countSend = countLed > (LED_COUNT - 2) ? LED_COUNT - 2 : countLed;
		memset(buf[numBuf], 0, 48);
		for (uint8_t i = 0; i < countSend; i++){
			if (_ledColor == _rgb)
				RGB2PWM(buf[numBuf], &(((RGB_t*)_ledBuf)[i]), i + 2);
			else
				HSV2PWM(buf[numBuf], &(((HSV_t*)_ledBuf)[i]), i + 2);
		}
	}else if (countSend < countLed){
		for (uint8_t i = 0; i < LED_COUNT; i++) {
			if (countSend < countLed) {
				if (_ledColor == _rgb)
					RGB2PWM(buf[numBuf], &(((RGB_t*)_ledBuf)[countSend]), i);
				else
					HSV2PWM(buf[numBuf], &(((HSV_t*)_ledBuf)[countSend]), i);
				countSend++;
			} else {
				memset(&(buf[numBuf][24 * i]), ~0, 24);
			}
		}
	}else{//countSend >= countLed
		memset(buf[numBuf], ~0, ARRAY_LEN_BUF);
		endTransfer--;
		if (!endTransfer){
			_ledBuf = 0;
		}
	}
}






WS2812B::WS2812B(TIM_HandleTypeDef *htim, uint32_t Channel, uint16_t countLed){
	this->htim = htim;
	this->Channel = Channel;
	this->countLed = countLed;

	memset(buf, ~0, sizeof(buf));

	countSend = 0;
	_ledBuf = 0;
}

void WS2812B::start(){
	numBuf = 1;
	HAL_TIM_PWM_Start_DMA(htim, Channel, (uint32_t*) buf, sizeof(buf));
}

void WS2812B::UpdateBufferFULL(){
	numBuf = 1;
	toBuf();
}

void WS2812B::UpdateBufferHALF(){
	numBuf = 0;
	toBuf();
}

void WS2812B::Update(RGB_t *rgb){
	_ledBuf = rgb;
	_ledColor = _rgb;
	countSend = 0;
}

void WS2812B::Update(HSV_t *hsv){
	_ledBuf = hsv;
	_ledColor = _hsv;
	countSend = 0;
}
