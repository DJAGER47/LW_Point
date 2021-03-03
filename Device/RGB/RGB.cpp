/*
 * RGB.cpp
 *
 *  Created on: 5 февр. 2020 г.
 *      Author: lavrovskij
 */

#include "RGB.h"
#include "main.h"
#include "define.h"
#include "version.h"

#define TIM_RGB htim4
extern TIM_HandleTypeDef TIM_RGB;

RGB::RGB(){
}

void RGB::init(){
	HAL_TIM_PWM_Start(&TIM_RGB, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_RGB, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_RGB, TIM_CHANNEL_3);
	printf("Start RGB\r\n");
}

void RGB::setColor(COLOR::_rgb &in){
	__HAL_TIM_SET_COMPARE(&TIM_RGB, TIM_CHANNEL_1, in.r * cfgSave.brightnessX);
	__HAL_TIM_SET_COMPARE(&TIM_RGB, TIM_CHANNEL_2, in.g * cfgSave.brightnessX);
	__HAL_TIM_SET_COMPARE(&TIM_RGB, TIM_CHANNEL_3, in.b * cfgSave.brightnessX);
}

void RGB::setColor(COLOR::_hsv& in){
	auto rgb = COLOR::hsv2rgb(in);
	setColor(rgb);
}

