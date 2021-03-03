/*
 * wifi.cpp
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: lavrovskij
 */
#include "define.h"
#include "wifi_protocol.h"
#include "wifi.h"

#include "uarthandler.h"
#include "version.h"
#include "../disk/Disk.h"
#include "main.h"

#include "cmsis_os.h"
#define EN_PIN  ESP_EN_Pin
#define EN_PORT ESP_EN_GPIO_Port

#define RX_PIN  ESP_RX_Pin
#define RX_PORT ESP_RX_GPIO_Port

//#define _delay HAL_Delay
#define _delay osDelay

STATUS_RECEIV status_receive_wifi;
//==============================================================================
//==============================================================================
static uint8_t WIFIrxData[lenRxData];

void wifi::waitWifi(){ // Использовать до инита UART
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ESP_TX_Pin | ESP_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ESP_TX_GPIO_Port, &GPIO_InitStruct);

	power(false);
	osDelay(10);
	power(true);

	while (HAL_GPIO_ReadPin(RX_PORT, RX_PIN) != GPIO_PIN_SET){
		osDelay(1);
	}

    GPIO_InitStruct.Pin = ESP_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ESP_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ESP_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ESP_RX_GPIO_Port, &GPIO_InitStruct);
    osDelay(100);
}

void wifi::init(){
	if (cfgSave.wifi_en)
		waitWifi();
	else
		power(false);

	uart_handler::init();
	phyDev.wifi_server_initOk = true;
	printf("Start wifi\r\n");
}

void wifi::power(bool en){
	GPIO_PinState bit = en ? GPIO_PIN_SET : GPIO_PIN_RESET;
	HAL_GPIO_WritePin(EN_PORT, EN_PIN, bit);
	phyDev.Real_Rssi = -128;
}


uint8_t* wifi::received() {
	static RECEIVED state = LEN_LSB;
	static uint16_t len;
	static uint8_t *ptr;
	static uint8_t bcc;
	static bool timeout(true);


	while (uart_handler::getLen()) {
		timeout = false;

		switch (state) {
		case LEN_LSB:
			bcc = 0;
			ptr = WIFIrxData;
			ptr[0] = uart_handler::del();
			bcc -= ptr[0];
			state = LEN_MSB;
			break;

		case LEN_MSB:
			ptr[1] = uart_handler::del();
			bcc -= ptr[1];
			len = *((uint16_t*) (ptr)) + 2;
			ptr += 2;

			if (len > lenRxData) {
				printf("len error\r\n");
				state = LEN_LSB;
			} else {
				state = DATA;
			}
			break;

		case DATA:
			*ptr = uart_handler::del();
			bcc -= *ptr;
			ptr++;
			len--;
			if (!len)
				state = BCC;
			break;

		case BCC:
			state = LEN_LSB;
			if (bcc == uart_handler::del()) {
				return WIFIrxData;
			} else {
				printf("pack error\r\n");
			}
			break;
		}
	}


	static uint32_t prevTim = osKernelGetTickCount() + 10;
	if (osKernelGetTickCount() > prevTim) {
		prevTim = osKernelGetTickCount() + 10;
		if (timeout && (state != LEN_LSB)) {
			printf("U wifi rst\r\n");
			uart_handler::reset();
			state = LEN_LSB;
		}
		timeout = true;
	}

	return 0;
}

void wifi::process(uint8_t* p){
	WIFI_DATA<0> *data = (WIFI_DATA<0>*) p;

	if (data->cmd == SYNC_TIME){ //TODO SYNC_TIME
		//при необходимости синхронизации часов реального времени
		//если хост заявлен как источник времени
		doSYNC_TIME(data);
	}else

	if (data->status == S_REQUEST){ //request
		switch (data->cmd){
			case CONNECT: 			doCONNECT(data); break;
			case SET_SERVER_CTRL: 	doSET_SERVER_CTRL(data); break;
			case START_GAME: 		doSTART_GAME(data); break; //при старте сетевой игры
			case STOP_GAME: 		doSTOP_GAME(data); break;  //при останове сетевой игры
			case WRITE_ITEMS: 		doWRITE_ITEMS(data); break;//при записи параметров

			case CREATE_FILE: 		doCREATE_FILE(data); break;
			case WRITE_FILE: 		doWRITE_FILE(data); break;
			case READ_FILE_INFO: 	doREAD_FILE_INFO(data); break;
			case EXECUTE_FILE: 		doEXECUTE_FILE(data); break;
			case DELETE_FILE: 		doDELETE_FILE(data); break;
			case GET_AUDIO_FILE_LIST: doGET_AUDIO_FILE_LIST(data); break;

			case REBOOT: 			HAL_NVIC_SystemReset(); break; //трасляция команды сервера
			default:
				SendWifi(data->cmd, E_INVALID_CMD, 0, 0, 0, 0);
				printf("ERROR unknown command request\r\n");
				break;
		}
	}else{ //answer
		switch (data->cmd){
			case SET_ENV: 			doSET_ENV(data); break;//при первом соединении с модулем
			case SET_CONFIG: 		doSET_CONFIG(data); break; //при первом соединении с модулем
			case UPDATE_ITEMS: 		doUPDATE_ITEMS(data); break;//для обновления изменившихся параметров (при получении статуса с ошибкой необходимо провести процедуру инициализации)
			case CLEAR_STAT: 		doCLEAR_STAT(data); break;//при получении команды ИК "сброс статистики"
			case UPDATE_EVENTS: 	doUPDATE_EVENTS(data); break;//для обновления архива статистики (при получении статуса с ошибкой необходимо провести процедуру инициализации)
			default:
				SendWifi(data->cmd, E_INVALID_CMD, 0, 0, 0, 0);
				printf("ERROR unknown command answer\r\n");
				break;
		}
	}
}

void wifi::st_init_wifi_process(){
	constexpr uint16_t delay = 100;
	static ST_INIT_WIFI st_init_wifi = ST_setSET_ENV;
	static uint32_t prevTime;

	switch (st_init_wifi){
	case ST_setSET_ENV:
		sendSET_ENV();
		prevTime = osKernelGetTickCount() + delay;
		st_init_wifi = ST_getSET_ENV;
		break;
	case ST_getSET_ENV:
		if (osKernelGetTickCount() > prevTime){
			st_init_wifi = ST_setSET_ENV;
			break;
		}
		if (status_receive_wifi.recieved){
			status_receive_wifi.recieved = false;
			st_init_wifi = ST_setSET_CONFIG;
			//status_receive_wifi.data ERROR?
		}
	break;


	case ST_setSET_CONFIG:
		sendSET_CONFIG();
		prevTime = osKernelGetTickCount() + delay;
		st_init_wifi = ST_getSET_CONFIG;
		break;
	case ST_getSET_CONFIG:
		if (osKernelGetTickCount() > prevTime) {

			st_init_wifi = ST_setSET_CONFIG;
			break;
		}
		if (status_receive_wifi.recieved) {
			status_receive_wifi.recieved = false;
			st_init_wifi = ST_setUPDATE_ITEMS;
			//status_receive_wifi.data ERROR?
		}
		break;


	case ST_setUPDATE_ITEMS:
		sendUPDATE_ITEMS_ALL();
		prevTime = osKernelGetTickCount() + delay;
		st_init_wifi = ST_getUPDATE_ITEMS;
		break;
	case ST_getUPDATE_ITEMS:
		if (osKernelGetTickCount() > prevTime) {
			st_init_wifi = ST_setUPDATE_ITEMS;
			break;
		}
		if (status_receive_wifi.recieved) {
			status_receive_wifi.recieved = false;
			st_init_wifi = ST_setSET_ENV;
			phyDev.wifi_server_initOk = false;
			//status_receive_wifi.data ERROR?
			printf("INIT WIFI OK\r\n");
		}
		break;

	}
}

void wifi::handler(){
	uint8_t *p = received();
	if (p)
		process(p);

	if (uItem.needUpdateItem && (uItem.time < osKernelGetTickCount())){
		sendUPDATE_ITEMS_ALL();
		Disk::saveCfg();
		uItem.needUpdateItem = false;
	}

	if (cfgSave.wifi_en && phyDev.wifi_server_initOk){
		st_init_wifi_process();
	}
}

