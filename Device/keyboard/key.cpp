/*
 * key.cpp
 *
 *  Created on: 19 дек. 2019 г.
 *      Author: lavrovskij
 */
#include "stm32f1xx_hal.h"
#include "key.h"
#include "define.h"
#include "../sound/Sound.h"
#include "main.h"

#include "cmsis_os.h"


struct KeyType {
		uint16_t io;
		GPIO_TypeDef *GPIO;
	};

constexpr KeyType io_key[4] = {
  { .io = KEY2_Pin, .GPIO = KEY2_GPIO_Port },
  { .io = KEY4_Pin, .GPIO = KEY4_GPIO_Port },
  { .io = KEY1_Pin, .GPIO = KEY1_GPIO_Port },
  { .io = KEY3_Pin, .GPIO = KEY3_GPIO_Port }};


Key::Key() :
		shortDelay(50),
		longDelay(500),
		exitDelay(5000),
		short_state{0},
		long_state{0},
		time_key{0},
		updown(false){
}

void Key::init(osMessageQueueId_t myKey){
	myKeyHandle = myKey;
}

void Key::handler(){
	uint8_t key1 = HAL_GPIO_ReadPin(io_key[Button::KEY_SHORT_LEFT_TOP].GPIO,  io_key[Button::KEY_SHORT_LEFT_TOP].io);
	uint8_t key2 = HAL_GPIO_ReadPin(io_key[Button::KEY_SHORT_RIGHT_TOP].GPIO, io_key[Button::KEY_SHORT_RIGHT_TOP].io);
	if (key1 && key2){
		updown = false;
		short_state[4] = 0;
	}else if ((!key1) && (!key2)){
		updown = true;
		short_state[Button::KEY_SHORT_LEFT_TOP] = 0;
		short_state[Button::KEY_SHORT_RIGHT_TOP] = 0;
	}


	if (updown){
		uint32_t ms = osKernelGetTickCount();

		if ((!key1) && (!key2) && (!short_state[4]) && ((ms - time_key[4]) > shortDelay)){
			short_state[4] = 1;
			time_key[4] = ms;
		} else if ((key1) && (!key2) && (short_state[4]) && ((ms - time_key[4]) > shortDelay)){
			short_state[4] = 0;
			time_key[4] = ms;
			eKEY k = UP;
			osMessageQueuePut(myKeyHandle, &k, 0, 0);
			printf("UP\r\n");
		} else if ((!key1) && (key2) && (short_state[4]) && ((ms - time_key[4]) > shortDelay)){
			short_state[4] = 0;
			time_key[4] = ms;
			eKEY k = DOWN;
			osMessageQueuePut(myKeyHandle, &k, 0, 0);
			printf("DOWN\r\n");
		}
	}else{
		uint8_t i = Button::KEY_SHORT_LEFT_TOP;
		for (;i < 2; ++i) //LEFT RIGHT
		{
			uint32_t ms = osKernelGetTickCount();
			uint8_t key_state = HAL_GPIO_ReadPin(io_key[i].GPIO, io_key[i].io);

			if ((key_state == 0) && (!short_state[i]) && ((ms - time_key[i]) > shortDelay)) {
				short_state[i] = 1;
				long_state[i] = 0;
				time_key[i] = ms;
			} else if ((key_state == 0) && (ms - time_key[i]) > longDelay) {
				long_state[i] = 1;
				time_key[i] = ms;
				uint8_t q = i+4;
				osMessageQueuePut(myKeyHandle, &q, 0, 0);
				printf("LONG %d\r\n", i);
			} else if ((key_state == 1) && (short_state[i]) && (ms - time_key[i]) > shortDelay) {
				short_state[i] = 0;
				time_key[i] = ms;

				if (!long_state[i]) {
					osMessageQueuePut(myKeyHandle, &i, 0, 0);
					printf("%d\r\n", i);
				}
			}
		}
	}

	uint8_t i = eKEY::OK;
	{
		uint32_t ms = osKernelGetTickCount();
		uint8_t key_state = HAL_GPIO_ReadPin(io_key[i].GPIO, io_key[i].io);

		if ((key_state == 0) && (!short_state[i]) && ((ms - time_key[i]) > shortDelay)) {
			short_state[i] = 1;
			time_key[i] = ms;
		} else if ((key_state == 1) && (short_state[i]) && (ms - time_key[i]) > shortDelay) {
			short_state[i] = 0;
			time_key[i] = ms;
			osMessageQueuePut(myKeyHandle, &i, 0, 0);
		}
	}

	i = eKEY::BACK; //back
	{
		uint32_t ms = osKernelGetTickCount();
		uint8_t key_state = HAL_GPIO_ReadPin(io_key[i].GPIO, io_key[i].io);

		if ((key_state == 0) && (!short_state[i]) && ((ms - time_key[i]) > shortDelay)) {
			short_state[i] = 1;
			long_state[i] = 0;
			time_key[i] = ms;
		} else if ((key_state == 0) && (ms - time_key[i]) > exitDelay) {
			long_state[i] = 1;
			uint8_t q = eKEY::EXIT;
			osMessageQueuePut(myKeyHandle, &q, 0, 0);
		} else if ((key_state == 1) && (short_state[i]) && (ms - time_key[i]) > shortDelay) {
			short_state[i] = 0;
			time_key[i] = ms;

			if (!long_state[i]) {
				osMessageQueuePut(myKeyHandle, &i, 0, 0);
			}
		}
	}

}
