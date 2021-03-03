/*
 * Init.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: Admin
 */

#include "Init.h"
#include "cmsis_os.h"

#include "version.h"

#include "../Device/disk/Disk.h"
#include "../Device/sound/Sound.h"

extern osMessageQueueId_t mySoundHandle;

void Init(){
	portENABLE_INTERRUPTS();
	initPhyDev();

	Disk::init(); //Вначале читаем конфиг, потом  все остальное

	Sound::init(mySoundHandle);
	Sound::addSong(effectPower_on);
	Sound::addSong(effectAfterPower, 500);
	Sound::addSong(voiceSwitchGameMode[Lang], 500);

	phyDev.Real_State = STATE_Ready;
	portDISABLE_INTERRUPTS();
}

