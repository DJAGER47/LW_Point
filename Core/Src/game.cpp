/*
 * game.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: lavrovskij
 */

#include "game.h"
#include "cmsis_os.h"
//==============================================================================
SSD1283A myDisp;
RGBRing ring;
RGB rgb;
//==============================================================================
GAME_SETUP gameSetup;

void game::handler(){
	Menu::startMenu();
	Menu::menuHandler();
	switch (cfgSave.Cfg_GameType) {
	//case 0: process_game1(); break;
	//case 1: process_game2(); break;
	//case 2: process_game3(); break;
	}
}

extern "C" void StartGame(void const *argument) { //TODO 1 StartGame
	printf("StartGame\r\n");
	myDisp.init();
	ring.init();
	rgb.init();

	/*myDisp.drawImage(0,0, &fimg_startup);
	while (Sound::isRun()){
		osDelay(10);
	}*/


	while (1){
		game::handler();
	}
}


