/*
 * Судный день
 *
 *  Created on: 9 мар. 2020 г.
 *      Author: Admin
 */
#include "game.h"


void g1BombTimer() {
	if (cfgSave.G1_ActivationFlags & FLAG_FLASH) { //До конца игры флешка должна стоять
		if (!Adc::getUSB()){
			Sound::addSong(effectDeviceDeactivated);
			Sound::addSong(voiceDeviceDeactivated[Lang]);
			gameSetup.st_game = G_ActivationBOMB;
			RGB::setColor(LED_BLACK);
			return;
		}
	}

	if (!Sound::isRun()) Sound::addSong(effectTicker);


	if (!AnimS7ReversTime(gameSetup.countTemp)) return;

	Sound::urgentlySong(effectExplosion);
	Sound::addSong(effectRoundCompleted[Lang]);
	Sound::addSong(effectVictory);

	STLED316S::clear();
	gameSetup.delayTimer = SysTicks;
	gameSetup.countTemp = cfgSave.numPulseExplosion;
	gameSetup.st_game = G_END;
	gameSetup.Win = true;
	gameSetup.imgEnd = &fimg_game1Win;
}


void game::process_game1() {
	zeroTimer();
	gameSetup.st_game = G_INIT;
	gameSetup.isExit = false;
	gameSetup.isPause = false;

	gameSetup.prevUSB = true;
	gameSetup.prevPass  = true;
	gameSetup.prevShot = true;
	gameSetup.showWin = true;
	gameSetup.Win = false;
	gameSetup.startStar = false;

	uint32_t prevClock = SysTicks;
	reinitPass();

	while(1){
		update();
		if (gameSetup.isExit) break;
		if (processGameExit()) break;

		if (!gameSetup.isPause){

			uint32_t time = cfgSave.RoundTime - ((SysTicks - gameSetup.tStartGame) / 1000);
			if (((gameSetup.st_game == G_ActivationDelay) ||
				 (gameSetup.st_game == G_ActivationBOMB) ||
				 (gameSetup.st_game == G_BombTimer))
					&& (SysTicks >= prevClock)) {

				prevClock = SysTicks + 1000;
				updateTimeGame(time);
				if ((time > (0xFFFFFFFF - 1000))) gameSetup.st_game = G_TIME_END; //(time == 0) ||
			}

			switch (gameSetup.st_game) {
				case G_INIT: gINIT(cfgSave.G1_ActivationDelay); break;
				case G_LGSD: gCfg_LGSD(cfgSave.G1_ActivationDelay); break;

			//GAME
			case G_ActivationDelay: gActivationDelay(); break; // Задержка активации
			case G_ActivationBOMB: gActivationBOMB(cfgSave.G1_ActivationFlags,
												   cfgSave.G1_Password,
												   cfgSave.G1_BombTimer * 60); break;
			case G_BombTimer: g1BombTimer(); break; //Отсчет взрыва бомбы
			//END GAME

				case G_END: gEnd(cfgSave.G1_ActivationFlags); break;

				case G_TIME_END: //нужно отрисовать и выйти
					Display::drawImageFull(&fimg_game1Losing);
					Sound::urgentlySong(effectRoundCompleted[Lang]);
					gameSetup.st_game = G_STOP;
					break;
				case G_STOP:
					AnimS7Bomb('-');
					rgbColor(LED_WHITE);
					break;
			}
		}
		Key::reinit();
	}
	STLED316S::clear();
	Menu::run_s1i1();
	phyDev.Real_State = STATE_Ready;
}
