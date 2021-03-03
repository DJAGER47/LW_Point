/*
 * adc.cpp
 *
 *  Created on: 6 февр. 2020 г.
 *      Author: Admin
 */
#include "stm32f1xx_hal.h"
#include "adc.h"
#include "main.h"

#define hadc    hadc1
extern ADC_HandleTypeDef hadc;

uint8_t Adc::getVoltage(){
	uint32_t adc = HAL_ADCEx_InjectedGetValue(&hadc, ADC_INJECTED_RANK_1);

	float v = ((float)adc * 3.3 * 3 / 4096.0);
	static float prev = 0;

	if (prev == 0)
		prev = v;
	else
		prev = (0.7 * prev) + (0.3 * v);

	uint8_t acc = (uint8_t)(41.667 * prev - 250);
	if (prev > 8.4) acc = 100;
	if (prev < 6)   acc = 0;
	return acc;
}


