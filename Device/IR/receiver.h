/*
 * receiver.h
 *
 *  Created on: 28 ���. 2019 �.
 *      Author: Admin
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <stdint.h>
#include "protocol.h"

class Receiver {
public:
	static void init();
	static void handler();

	static void enable();
	static void disable();
	static bool isEmptyShot();
	static IRR_DATA getDataShot();
	static void reinitShot();
};



#endif /* RECEIVER_H_ */
