/*
 * wifi.h
 *
 *  Created on: 13 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef WIFI_UART2HANDLER_H_
#define WIFI_UART2HANDLER_H_

#include <stdint.h>

constexpr uint16_t SIZE_UART = (1024 * 4);

class uart_handler {
public:
	static void init();

	static uint8_t* getBufTx();
	static bool busyTX();
	static void startSend(uint16_t);

	static uint8_t del();
	static uint16_t getLen();
	static void reset();
};

#endif /* WIFI_UART2HANDLER_H_ */
