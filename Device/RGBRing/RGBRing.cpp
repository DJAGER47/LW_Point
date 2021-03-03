/*
 * RGBRing.cpp
 *
 *  Created on: Feb 10, 2021
 *      Author: Admin
 */

#include "RGBRing.h"
#include "main.h"
#include "biteArray.h"
#include "version.h"


#define G_HIGH()     HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_SET)
#define G_LOW()      HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_RESET)

#define RCK_HIGH()   HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_SET)
#define RCK_LOW()    HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_RESET)

#define SRCK_HIGH()  HAL_GPIO_WritePin(SRCK_GPIO_Port, SRCK_Pin, GPIO_PIN_SET)
#define SRCK_LOW()   HAL_GPIO_WritePin(SRCK_GPIO_Port, SRCK_Pin, GPIO_PIN_RESET)

#define SERIN_HIGH() HAL_GPIO_WritePin(SERIN_GPIO_Port, SERIN_Pin, GPIO_PIN_SET)
#define SERIN_LOW()  HAL_GPIO_WritePin(SERIN_GPIO_Port, SERIN_Pin, GPIO_PIN_RESET)

#define TIM_RING htim1
extern TIM_HandleTypeDef TIM_RING;

inline void delay(){
	volatile uint8_t d = 10;
	while (d--);
}

RGBRing::RGBRing() :
		countLed(48),
		led{0} {
}

void RGBRing::init(){
	G_HIGH();
	RCK_LOW();
	SRCK_LOW();
	SERIN_LOW();

	HAL_TIM_PWM_Start(&TIM_RING, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_RING, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM_RING, TIM_CHANNEL_3);
}

void RGBRing::writeLed(uint8_t num, bool enable){
	SetBit((uint8_t*)&led, num, enable);
}

void RGBRing::update(){
	//G_HIGH();
	RCK_LOW();
	SERIN_LOW();
	for (auto i = 0; i < countLed; i++){
		SRCK_LOW();
		if(GetBit((uint8_t*)&led, i))
			SERIN_HIGH();
		else
			SERIN_LOW();
		delay();
		SRCK_HIGH();
		delay();
	}
	SRCK_LOW();
	RCK_HIGH();
	delay();
	RCK_LOW();
	delay();
	G_LOW();
}

void RGBRing::setColor(COLOR::_rgb& in){
	__HAL_TIM_SET_COMPARE(&TIM_RING, TIM_CHANNEL_1, in.r * cfgSave.brightnessX);
	__HAL_TIM_SET_COMPARE(&TIM_RING, TIM_CHANNEL_2, in.g * cfgSave.brightnessX);
	__HAL_TIM_SET_COMPARE(&TIM_RING, TIM_CHANNEL_3, in.b * cfgSave.brightnessX);
}

void RGBRing::setColor(COLOR::_hsv& in){
	auto rgb = COLOR::hsv2rgb(in);
	setColor(rgb);
}
