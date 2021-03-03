/*
 * wifi_send.cpp
 *
 *  Created on: 16 мар. 2020 г.
 *      Author: lavrovskij
 */

#include "wifi_protocol.h"
#include "uarthandler.h"

void SendWifi(WIFI_Cmd cmd, WIFI_Status status, uint8_t len1, void* data1, uint8_t len2, void* data2){
	uint16_t size = len1 + len2;
	uint16_t len = size + sizeof(WIFI_DATA<0>);

	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<0> *data = (WIFI_DATA<0>*) BufTx;
	data->len = size;
	data->cmd = cmd;
	data->status = status;

	memcpy(data->data, data1, len1);
	memcpy(data->data + len1, data2, len2);

	data->data[size] = 0;
	for (uint16_t i = 0; i < (len - 1); i++)
		data->data[size] -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}

