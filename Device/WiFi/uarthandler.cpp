/*
 * wifi.cpp
 *
 *  Created on: 13 февр. 2020 г.
 *      Author: lavrovskij
 */
#include "stm32f1xx_hal.h"
#include "define.h"
#include "uarthandler.h"


#define dmaRx hdma_uart4_rx
#define dmaTx hdma_uart4_tx
#define uart  huart4

extern UART_HandleTypeDef uart;
extern DMA_HandleTypeDef dmaRx;
extern DMA_HandleTypeDef dmaTx;

constexpr uint16_t errorU4 = SIZE_UART*0.9;

template<class T, uint16_t size> class
uQueue {
private:
	T pBuf[size];

	uint16_t ukEnd;
	uint16_t ukBegin;

public:
	uQueue() : ukEnd(0), ukBegin(0)  {
	}

	void reset(){
		ukBegin = ukEnd;
	}

	T del(){//Прочитать и достать голову
		T data = pBuf[ukBegin];
		ukBegin = (ukBegin + 1) % size;
		return data;
	}

	uint16_t getLen(){
		ukEnd = size - ((uint16_t)dmaRx.Instance->CNDTR);
		uint16_t len = ukEnd >= ukBegin ? (ukEnd - ukBegin) : (size - ukBegin + ukEnd);
		if (len>(errorU4))
			printf("U4 90%%\r\n");
		return  len;
	}

	T* getBuf(){
		return pBuf;
	}
};



static uQueue<uint8_t, SIZE_UART> UrxData;
static uint8_t UtxData[SIZE_UART];

void uart_handler::init(){
	HAL_UART_Receive_DMA(&uart, UrxData.getBuf(), SIZE_UART);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	if (huart == &uart){ //if framing error
		printf("Framing error\r\n");
		HAL_UART_Receive_DMA(&uart, UrxData.getBuf(), SIZE_UART);
	}
}


///SEND
uint8_t* uart_handler::getBufTx(){
	return UtxData;
}

bool uart_handler::busyTX(){
	return (bool)(HAL_DMA_GetState(&dmaTx) != HAL_DMA_STATE_READY);
}

void uart_handler::startSend(uint16_t len){
	HAL_UART_Transmit_DMA(&uart, UtxData, len);
}


//RECEIVE
uint8_t uart_handler::del(){//Прочитать и достать голову
	return UrxData.del();
}

uint16_t uart_handler::getLen(){
	return UrxData.getLen();
}

void uart_handler::reset(){
	UrxData.reset();
}

