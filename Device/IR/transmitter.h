/*
 * transmitter.h
 *
 *  Created on: Jan 6, 2020
 *      Author: Admin
 */

#ifndef IR_TRANSMITTER_H_
#define IR_TRANSMITTER_H_

#include "protocol.h"

#define ms2400 136
#define ms1200 68
#define ms600 34

#define msGap 33

class Transmitter {
	static void Init_TIM_Slave();
	static void Init_TIM_Master();
	static void sendHardware(uint8_t len);
public:
	static void init(uint8_t power);
	static bool busy();
	static void send(IRR_DATA data);
	static void updPower(uint8_t pwr);
};

#endif /* IR_TRANSMITTER_H_ */
