/*
 * Others.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: Admin
 */
#include "Others.h"
#include "define.h"
#include "cmsis_os.h"

#include "../Device/WiFi/wifi.h"
#include "../Device/sound/Sound.h"
#include "../Device/IR/receiver.h"
#include "../Device/IR/transmitter.h"
#include "../Device/keyboard/key.h"
#include "../Device/ssd1283A/ssd1283A.h"
#include "../Device/adc/adc.h"

#include "version.h"
#include "bmp.h"

extern osMessageQueueId_t myKeyHandle;
//==============================================================================
Key key;
//==============================================================================
extern SSD1283A myDisp;

//==============================================================================

void updateTopBar(){
	static uint32_t prev_ms = 0;
	uint32_t ms = osKernelGetTickCount();
	if (prev_ms > ms) return;
	prev_ms = ms + 1000;

		const tImage *icon;
		if (cfgSave.wifi_en){
			if (phyDev.Real_Rssi >= -50)		icon = &img_rssi5;
			else if (phyDev.Real_Rssi >= -60)	icon = &img_rssi4;
			else if (phyDev.Real_Rssi >= -70)	icon = &img_rssi3;
			else if (phyDev.Real_Rssi >= -90)	icon = &img_rssi2;
			else if (phyDev.Real_Rssi >= -100)	icon = &img_rssi1;
			else								icon = &img_rssi0;
		}else{
			icon = &img_rssiOff;
		}
		myDisp.drawImage(0, 0, icon, false);


		uint8_t acc = Adc::getVoltage();
		phyDev.Real_ACClevel = acc;
		if (acc >= 80)		 icon = &img_bat6;
		else if (acc >= 65)  icon = &img_bat5;
		else if (acc >= 55)  icon = &img_bat4;
		else if (acc >= 45)  icon = &img_bat3;
		else if (acc >= 35)  icon = &img_bat2;
		else if (acc >= 25)  icon = &img_bat1;
		else				 icon = &img_bat0;
		myDisp.drawImage(DISP_W - icon->width, 0, icon, false);
}

extern "C" void StartOther(void *argument){ //TODO 2 StartOthers
	printf("StartOther\r\n");

	wifi::init();
	key.init(myKeyHandle);
	//Transmitter::init(cfgSave.Cfg_IRpower);
	while(1){
		wifi::handler();
		Sound::handle();

		key.handler();
		//Receiver::handler();

		updateTopBar();
		//LED
		//resive
		//transmit
		osDelay(1);
	}
}
