/*
 * key.h
 *
 *  Created on: 19 дек. 2019 г.
 *      Author: lavrovskij
 */

#ifndef KEY_H_
#define KEY_H_

#include <stdint.h>
#include "cmsis_os.h"


class Key {
	const uint8_t shortDelay;
	const uint16_t longDelay;
	const uint16_t exitDelay;

	uint8_t short_state[5];
	uint8_t long_state[5];
	uint32_t time_key[5];

	osMessageQueueId_t myKeyHandle;
	bool updown;

	enum Button {
			KEY_SHORT_LEFT_TOP = 0,
			KEY_SHORT_RIGHT_TOP = 1,

			KEY_SHORT_LEFT_BOTTOM = 2,
			KEY_SHORT_RIGHT_BOTTOM = 3,


			KEY_LONG_LEFT_TOP, //+4
			KEY_LONG_RIGHT_TOP,

			KEY_LONG_LEFT_BOTTOM,
			KEY_LONG_RIGHT_BOTTOM,
	};
public:
	enum eKEY {
		LEFT = Button::KEY_SHORT_LEFT_TOP,
		LONG_LEFT = Button::KEY_LONG_LEFT_TOP,

		RIGTH = Button::KEY_SHORT_RIGHT_TOP,
		LONG_RIGHT = Button::KEY_LONG_RIGHT_TOP,

		OK = Button::KEY_SHORT_LEFT_BOTTOM,
		BACK = Button::KEY_SHORT_RIGHT_BOTTOM,
		EXIT = Button::KEY_LONG_RIGHT_BOTTOM,

		UP,
		DOWN,
	};

	Key();
	void init(osMessageQueueId_t myKey);
	void handler();
};

#endif /* KEY_H_ */
