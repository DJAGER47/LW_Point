/*
 * receiver.cpp
 *
 *  Created on: 28 дек. 2019 г.
 *      Author: Admin
 */

#include "receiver.h"
#include "define.h"

#include "version.h"
#include "game.h"

#include "main.h"

#define TIM_IRR htim2
extern TIM_HandleTypeDef TIM_IRR;


#define ms24max 250
#define ms24min 230

#define ms12max 130
#define ms12min 110

#define ms6max 70
#define ms6min 50


extern osMessageQueueId_t myIRRcmdHandle;
extern osMessageQueueId_t myIRRshotHandle;
extern osMessageQueueId_t myIRrawHandle;

void process(CNT& cnt){
	static IRR_DATA data = {.data = 0};
	static ST_IRR st = IRR_START;
	static uint8_t i;
	static uint8_t len;

	switch(st){
	case IRR_START:
		if ((cnt.cnt2 < ms24min) || (cnt.cnt2 > ms24max)) return;
		data.data = 0;
		st = IRR_CMD_SHOT;
		break;

	case IRR_CMD_SHOT:
		if (((cnt.cnt1 < ms6min) || (cnt.cnt1 > ms6max)) &&
		    ((cnt.cnt2 < ms6min) || (cnt.cnt2 > ms12max))){
			st = IRR_START;
		}else{
			if (cnt.cnt2 > ms12min) {
				len = bufDataR;
				data.data |= 1;
			} else {
				len = bufShotR;
			}
			i = 1;
			st = IRR_READ;
		}
		break;

	case IRR_READ:
		if (((cnt.cnt1 < ms6min) || (cnt.cnt1 > ms6max)) &&
		    ((cnt.cnt2 < ms6min) || (cnt.cnt2 > ms12max))){
			st = IRR_START;
		}else{
			data.data <<= 1;
			if (cnt.cnt2 > ms12min)
				data.data |= 1;
			i++;

			if (len == i) {
				if (len == bufShotR) {
					data.data <<= 10;
					osMessageQueuePut(myIRRshotHandle, &data, 0, 10);
					printf("Shot\r\n");
				} else {
					osMessageQueuePut(myIRRcmdHandle, &data, 0, 10);
					printf("Cmd\r\n");
				}
				st = IRR_START;
			}
		}
		break;
	}
}

extern "C" __RamFunc void TIM2_IRQHandler() {
	uint16_t itstatus = TIM2->SR & TIM_IT_CC1;
	uint16_t itenable = TIM2->DIER & TIM_IT_CC1;
	if ((itstatus != (uint16_t) RESET) && (itenable != (uint16_t) RESET)) {
		TIM2->SR = (uint16_t) ~TIM_IT_CC1;
		TIM2->SR = (uint16_t) ~TIM_IT_UPDATE;

		uint16_t cnt1 = TIM2->CCR1;
		uint16_t cnt2 = TIM2->CCR2;

		cnt1 -= cnt2;
		if (cnt1 > 255) cnt1 = 255;
		if (cnt2 > 255) cnt2 = 255;

		if ((cnt1 == 0) || (cnt2 == 0)) return;
		if (osMessageQueueGetSpace(myIRrawHandle)) {
			CNT cnt = { .cnt1 = (uint8_t) cnt2, .cnt2 = (uint8_t) cnt1 };
			osMessageQueuePut(myIRrawHandle, &cnt, 0, 0);
		}
	}
}

void Receiver::init() {
	HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_IC_Start_IT(&TIM_IRR, TIM_CHANNEL_1);
	HAL_TIM_IC_Start(&TIM_IRR, TIM_CHANNEL_2);

/*	 // Разрешаем таймеру генерировать прерывание по захвату
	TIM_IRR.Instance->DIER |= TIM_IT_CC1; //TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	TIM_IRR.Instance->SR = (uint16_t)~TIM_FLAG_CC1;//TIM_ClearFlag(TIM2, TIM_FLAG_CC1);
	TIM_IRR.Instance->SR = (uint16_t)~TIM_FLAG_CC2;//TIM_ClearFlag(TIM2, TIM_FLAG_CC2);

	HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	TIM_IRR.Instance->CR1 |= TIM_CR1_CEN;//TIM_Cmd(TIM2, ENABLE);
*/
	printf("Start Receiver\r\n");
}

void Receiver::handler(){

	CNT cnt;
	while (osMessageQueueGetCount(myIRrawHandle)){
		osMessageQueueGet(myIRrawHandle, &cnt, 0,0);
		process(cnt);
	}

	/*if (fifoIRcmd.getLength()){
		IRR_DATA data = fifoIRcmd.del();

		switch (data.cmd.byte1){
		case irCMD:

			switch (data.cmd.byte2){

			case irStgDefauld:
				if (!((phyDev.Real_State == STATE_PowerOn) || (phyDev.Real_State == STATE_Ready))) break;
				INFO("CONFIG DEFAULT\r\n");
				memcpy(&cfgSave, &cfgSaveDefault, sizeof(ConfigDevSave));
				break;


			case irStart:
			case irNewGame:
			case irNewGameDelay:{
				if ((phyDev.Real_State == STATE_RunGame) || (phyDev.Real_State == STATE_WaitStart)) break;
				if (data.cmd.byte2 == irNewGameDelay)
					gameSetup.LocalGameStartDelay = true;
				else
					gameSetup.LocalGameStartDelay = false;
				gameSetup.StartGame = true;
			}break;


			case irPause:
				if (!(phyDev.Real_State == STATE_RunGame)) break;

				if (gameSetup.isPause){ // пауза, снять
					gameSetup.tStartGame += SysTicks - gameSetup.timePause;
				}else{ //Снят с паузы, поставить
					gameSetup.timePause = SysTicks;
				}
				gameSetup.isPause = !gameSetup.isPause;
				break;


			case irAdminKill:
			case irStop:
				if (!((phyDev.Real_State == STATE_RunGame) || (phyDev.Real_State == STATE_WaitStart))) break;
				Sound::Stop();
				gameSetup.isExit = true;
				break;


			case irDeactivation: //TODO
				if (phyDev.Real_State != STATE_RunGame) break;

				break;

			case irActivation: //TODO
				if (phyDev.Real_State != STATE_RunGame) break;

				break;

			case irClearStat: //TODO Clear stat
				break;


			case irSwitchPower:
				//Сменить мощность выстрела (если была меньше 50% то станет 99%, если была больше 50% то станет 49%)
				cfgSave.Cfg_IRpower = cfgSave.Cfg_IRpower < 50 ? 99 : 49;
				Transmitter::updPower(cfgSave.Cfg_IRpower);
				Disk::saveCfg();
				break;
			case irPowerIn:
				cfgSave.Cfg_IRpower = 5;
				Transmitter::updPower(cfgSave.Cfg_IRpower);
				Disk::saveCfg();
				break;
			case irPowerOut:
				cfgSave.Cfg_IRpower = 100;
				Transmitter::updPower(cfgSave.Cfg_IRpower);
				Disk::saveCfg();
				break;


			case irWifiOff:
				cfgSave.wifi_en = false;
				wifi::power(false);
				Disk::saveCfg();
				break;
			case irWifiOn:
				cfgSave.wifi_en = true;
				wifi::waitWifi();
				phyDev.wifi_server_initOk = true;
				Disk::saveCfg();
				break;
			}
			break;

		case irPower:
			cfgSave.Cfg_IRpower = data.cmd.byte2;
			Transmitter::updPower(cfgSave.Cfg_IRpower);
			Disk::saveCfg();
			break;

		case irTimeRound:
			if (!((phyDev.Real_State == STATE_PowerOn) || (phyDev.Real_State == STATE_Ready))) break;
			cfgSave.RoundTime = data.cmd.byte2 * 60;
			Disk::saveCfg();
			break;
		}
	}*/
}

void Receiver::enable(){
	TIM_IRR.Instance->CR1 |= TIM_CR1_CEN;
}

void Receiver::disable(){
	TIM_IRR.Instance->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
}

bool Receiver::isEmptyShot(){
	return (osMessageQueueGetCount(myIRRshotHandle) == 0);
}

IRR_DATA Receiver::getDataShot(){
	static IRR_DATA data;
	data = {0};
	if (osMessageQueueGetCount(myIRRshotHandle))
		osMessageQueueGet(myIRRshotHandle, &data, 0, 0);
	return data;
}

void Receiver::reinitShot(){
	osMessageQueueReset(myIRRshotHandle);
}


