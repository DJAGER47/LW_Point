/*
 * wifi_protocol.cpp
 *
 *  Created on: 24 февр. 2020 г.
 *      Author: Admin
 */

#include "wifi_protocol.h"
#include "game.h"
#include "uarthandler.h"
#include "cmsis_os.h"

UPDATE_ITEM uItem = { 0, false };


//bidirectional------------------------------------------------------------------------
void sendSYNC_TIME(){

}
void doSYNC_TIME(WIFI_DATA<0>* data){
	if (data->status & 0x80) {
		printf("SYNC_TIME ERROR\r\n");
	} else {
		//printf("SYNC_TIME_OK %d\r\n", data->len);
	}
}

//от хоста в модуль:------------------------------------------------------------------------
void sendSET_ENV(){ //+
	constexpr uint16_t size = sizeof(_SET_ENV);
	constexpr uint16_t len = sizeof(WIFI_DATA<size>);
	while (uart_handler::busyTX());
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<size> *data = (WIFI_DATA<size>*) BufTx;
	data->len = size;
	data->cmd = SET_ENV;
	data->status = S_REQUEST;

	_SET_ENV *p = (_SET_ENV*) data->data;
	p->DeviceID = 0x02000502; //Идентификатор комплекса
	p->SyncSource = 0;//Источник времени для модуля/ 0-сервер/ 1-хост
	p->BLE_En = 0;//Включение модуля BLE

	data->bcc = 0;
	for (uint16_t i = 0; i < (len - 1); i++) data->bcc -= BufTx[i];


	uart_handler::startSend(len);
}
void doSET_ENV(WIFI_DATA<0>* data){
	printf("SET_ENV\r\n");
	status_receive_wifi.recieved = true;
	status_receive_wifi.data = data;

	if (data->status & 0x80){
		printf("SET_ENV ERROR\r\n");
	}
}

void sendSET_CONFIG(){//+
	constexpr uint16_t size = sizeof(_SET_CONFIG);
	constexpr uint16_t len = sizeof(WIFI_DATA<size>);
	while (uart_handler::busyTX());
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<size> *data = (WIFI_DATA<size>*) BufTx;
	data->len = size;
	data->cmd = SET_CONFIG;
	data->status = S_REQUEST;

	_SET_CONFIG *p = (_SET_CONFIG*) data->data;
	memcpy(p->SSID, cfgSave.wifi_ssid, 32);
	memcpy(p->Key, cfgSave.wifi_key, 32);


	data->bcc = 0;
	for (uint16_t i = 0; i < (len - 1); i++) data->bcc -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}
void doSET_CONFIG(WIFI_DATA<0>* data){
	printf("SET_CONFIG\r\n");
	if (!(data->status & 0x80)) {
		status_receive_wifi.recieved = true;
		status_receive_wifi.data = data;
	} else {
		printf("SET_CONFIG ERROR\r\n");
	}
}

void sendUPDATE_ITEMS_ALL(){ //+
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<0>* data = (WIFI_DATA<0>*) BufTx;
	_UPDATE_ITEMS* p = (_UPDATE_ITEMS*) data->data;

	p->Handle = osKernelGetTickCount();
	status_receive_wifi.handle = p->Handle;

	uint16_t index = 0;
	for (uint32_t i = 0; i < CfgItems.ItemCount; i++) {
		if (CfgItems.List[i].index) {
			p->data[index++] = i; //Index

			uint8_t len;
			if (CfgItems.List[i].size == 32) // Пароли и названия (32 максимальная длинна)
				len = *((uint8_t*) p_cfgDevSave[CfgItems.List[i].index]) + 1;
			else
				len = CfgItems.List[i].size;

			for (auto j = 0; j < len; j++)
				p->data[index++] = *((uint8_t*) p_cfgDevSave[CfgItems.List[i].index] + j);
		}
	}

	uint16_t size = index + sizeof(_UPDATE_ITEMS);//4
	uint16_t len = size + sizeof(WIFI_DATA<0>);//5
	data->len = size;
	data->cmd = UPDATE_ITEMS;
	data->status = S_REQUEST;

	data->data[size] = 0;
	for (uint16_t i = 0; i < (len - 1); i++)
		data->data[size] -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}
void doUPDATE_ITEMS(WIFI_DATA<0>* data){
	printf("UPDATE_ITEMS\r\n");
	if (status_receive_wifi.handle == *((uint32_t*)data->data)){
		status_receive_wifi.recieved = true;
		status_receive_wifi.data = data;

		if (data->status & 0x80) {
			printf("UPDATE_ITEMS ERROR\r\n");
		}
	}else{
		printf("UPDATE_ITEMS ERROR_HANDLE\r\n");
	}
}

void sendUPDATE_ITEMS_ONE(uint8_t i){
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<0> *data = (WIFI_DATA<0>*) BufTx;
	_UPDATE_ITEMS* p = (_UPDATE_ITEMS*) data->data;

	p->Handle = osKernelGetTickCount();
	//status_receive_wifi.handle = p->Handle;

	uint16_t index = 0;
	if (CfgItems.List[i].index) {
		p->data[index++] = i; //Index

		uint8_t len;
		if (CfgItems.List[i].size == 32) // Пароли и названия
			len = *((uint8_t*) p_cfgDevSave[CfgItems.List[i].index]) + 1;
		else
			len = CfgItems.List[i].size;

		for (auto j = 0; j < len; j++)
			p->data[index++] = *((uint8_t*) p_cfgDevSave[CfgItems.List[i].index] + j);
	}


	uint16_t size = index + sizeof(_UPDATE_ITEMS); //4
	uint16_t len = size + sizeof(WIFI_DATA<0> ); //5
	data->len = size;
	data->cmd = UPDATE_ITEMS;
	data->status = S_REQUEST;

	data->data[size] = 0;
	for (uint16_t i = 0; i < (len - 1); i++)
		data->data[size] -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}

//СТАТИСТИКА
void sendCLEAR_STAT(){
	constexpr uint16_t size = 4;
	constexpr uint16_t len = sizeof(WIFI_DATA<size> ) - 1;
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<size> *data = (WIFI_DATA<size>*) BufTx;
	data->len = size;
	data->cmd = GET_FW_DATA;
	data->status = S_REQUEST;

	_SET_ENV *p = (_SET_ENV*) data->data;
	//p->DeviceID = ;

	data->bcc = 0;
	for (uint8_t i = 0; i < (len); i++)
		data->bcc -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}
void doCLEAR_STAT(WIFI_DATA<0>* data){
	if (data->status & 0x80) {
		printf("CLEAR_STAT ERROR\r\n");
	} else {
		printf("CLEAR_STAT_OK %d\r\n", data->len);
	}
}

void sendUPDATE_EVENTS(){
	constexpr uint16_t size = 4;
	constexpr uint16_t len = sizeof(WIFI_DATA<size> ) - 1;
	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<size> *data = (WIFI_DATA<size>*) BufTx;
	data->len = size;
	data->cmd = GET_FW_DATA;
	data->status = S_REQUEST;

	_SET_ENV *p = (_SET_ENV*) data->data;
	//p->DeviceID = ;

	data->bcc = 0;
	for (uint8_t i = 0; i < (len); i++)
		data->bcc -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}
void doUPDATE_EVENTS(WIFI_DATA<0>* data){
	if (data->status & 0x80) {
		printf("UPDATE_EVENTS ERROR\r\n");
	} else {
		printf("UPDATE_EVENTS_OK %d\r\n", data->len);
	}
}
//END СТАТИСТИКА


//от модуля в хост:------------------------------------------------------------------------
void doCONNECT(WIFI_DATA<0>* data) {//+
	phyDev.Real_Rssi = data->data[0];
	SendWifi(CONNECT, S_OK, 0, 0, 0, 0);
}


void doSET_SERVER_CTRL(WIFI_DATA<0>* data) { //
	printf("SET_SERVER_CTRL\r\n");
	if (phyDev.Real_State == STATE_RunGame){
		SendWifi(SET_SERVER_CTRL, E_ACCESS_DENIED, 1, &phyDev.Real_State, 0, 0);
		return;
	}

	gameSetup.SrvCtrl = data->data[0];
	SendWifi(SET_SERVER_CTRL, S_OK, 1, &phyDev.Real_State, 0, 0);
}
void doSTART_GAME(WIFI_DATA<0>* data) { //при старте сетевой игры
	printf("START_GAME\r\n");
	/*if(get_state()==DEV_ST_RUN)
				uart_send(AT_COMM_CMD_START_GAME,LW_E_ACCESS_DENIED,1,&CurVal.State,0,0);
			else
			{
				game_start(RecvPack.Times[0],RecvPack.Times[1]);
				uart_send(AT_COMM_CMD_START_GAME,0,1,&CurVal.State,0,0);
			}*/
}
void doSTOP_GAME(WIFI_DATA<0>* data) { //при останове сетевой игры
	printf("STOP_GAME\r\n");
	/*game_stop(AT_EVT_FLAG_CMD);
			uart_send(AT_COMM_CMD_STOP_GAME,0,1,&CurVal.State,0,0);*/
}

void doWRITE_ITEMS(WIFI_DATA<0>* data) { //при записи параметров
	printf("WRITE_ITEMS\r\n");
	if(phyDev.Real_State == STATE_RunGame){
		SendWifi(WRITE_ITEMS, E_ACCESS_DENIED, 0, 0, 0, 0);
		return;
	}

	_WRITE_ITEMS *p = (_WRITE_ITEMS*) data->data;

	if (data->len > 4) {
		uint16_t len = data->len - 4;	//-Handle
		uint16_t index = 0;
		while (index < len) {
			uint8_t i = p->data[index++];

			if ((i > CfgItems.ItemCount) || (CfgItems.List[i].sType.Writeable == 0)) {
				printf("WRITE_ITEMS ERROR\r\n");
				SendWifi(WRITE_ITEMS, E_OUT_OF_RANGE, sizeof(p->Handle), &p->Handle, 0, 0);
				return;
			}

			uint8_t size = CfgItems.List[i].size;
			if (size == 32)
				size = p->data[index] + 1;
			for (auto j = 0; j < size; j++)
				*((uint8_t*) p_cfgDevSave[CfgItems.List[i].index] + j) = p->data[index++];
		}
	}
	//Request
	SendWifi(WRITE_ITEMS, S_OK, sizeof(p->Handle), &p->Handle, 0, 0);
	uItem.time = osKernelGetTickCount() + 100;
	uItem.needUpdateItem = true;
}

