/*
 * wifi.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef WIFI_WIFI_H_
#define WIFI_WIFI_H_

#include <stdint.h>

typedef enum {//RECEIVED
	LEN_LSB = 0, LEN_MSB, DATA, BCC//CMD, STATUS , _ERROR, _ERROR2
} RECEIVED;


enum ST_INIT_WIFI{
	ST_setSET_ENV,
	ST_getSET_ENV,

	ST_setSET_CONFIG,
	ST_getSET_CONFIG,

	ST_setUPDATE_ITEMS,
	ST_getUPDATE_ITEMS
};

class wifi {
#define lenRxData 2053

private:
	static uint8_t* received();
	static void process(uint8_t* p);
	static void st_init_wifi_process();

public:
	static void init();
	static void handler();
	static void power(bool en);
	static void waitWifi();
	static void updateFW();
};


#endif /* WIFI_WIFI_H_ */
