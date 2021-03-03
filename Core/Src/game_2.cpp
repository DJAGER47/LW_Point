/*
 * Грязная бомба
 *
 *  Created on: 9 мар. 2020 г.
 *      Author: Admin
 */
#include "game.h"
//gameSetup.countTemp


void g2BombTimer() {
	/*if (cfgSave.G2_ActivationFlags & FLAG_FLASH) { //До конца игры флешка должна стоять
		if (!Adc::getUSB()){
			Sound::addSong(effectDeviceDeactivated);
			Sound::addSong(voiceDeviceDeactivated[Lang]);
			gameSetup.st_game = G_ActivationDelay;
			return;
		}
	}*/

	if (Sound::isEmpty()) Sound::addSong(effectTicker);


	//DEACTIVATION
	if (!AnimS7ReversTime(gameSetup.countTemp)){
		/*if (cfgSave.G2_DeactivationFlags & FLAG_FLASH) {
		}*/

		if (cfgSave.G2_DeactivationFlags & FLAG_PASS)
			if (!processPass(cfgSave.G2_Password))
				return;

		if (cfgSave.G2_DeactivationFlags & FLAG_SHOT)
			if (!processShot())
				return;

		//Разминирование
		Sound::addSong(effectDeviceDeactivated);
		Sound::addSong(voiceDeviceDeactivated[Lang]);
		Sound::addSong(effectRoundCompleted[Lang]);
		Sound::addSong(effectVictory);

		STLED316S::clear();
		gameSetup.delayTimer = SysTicks;
		gameSetup.countTemp = 0;
		gameSetup.st_game = G_END;
		gameSetup.Win = false;
		gameSetup.imgEnd = &fimg_game2Losing;
		return;
	}
	//Взрыв
	//////////////
	Sound::urgentlySong(effectExplosion);
	Sound::addSong(effectRoundCompleted[Lang]);
	Sound::addSong(effectVictory);

	STLED316S::clear();
	gameSetup.delayTimer = SysTicks;
	gameSetup.countTemp = cfgSave.numPulseExplosion;
	gameSetup.st_game = G_END;
	gameSetup.Win = true;
	gameSetup.imgEnd = &fimg_game2Win;
}


void game::process_game2(){
	zeroTimer();
	gameSetup.st_game = G_INIT;
	gameSetup.isExit = false;
	gameSetup.isPause = false;

	gameSetup.prevUSB = true;
	gameSetup.prevPass = true;
	gameSetup.prevShot = true;
	gameSetup.showWin = true;
	//gameSetup.Win = false; //не требуеться
	gameSetup.startStar = false;

	uint32_t prevClock = SysTicks;
	reinitPass();

	while (1) {
		update();
		if (gameSetup.isExit)  break;
		if (processGameExit()) break;

		if (!gameSetup.isPause) {
			uint32_t time = cfgSave.RoundTime - ((SysTicks - gameSetup.tStartGame) / 1000);
			if (((gameSetup.st_game == G_ActivationDelay)
					|| (gameSetup.st_game == G_ActivationBOMB)
					|| (gameSetup.st_game == G_BombTimer))
					&& (SysTicks >= prevClock)) {

				prevClock = SysTicks + 1000;
				updateTimeGame(time);
				if (time == 0) gameSetup.st_game = G_TIME_END;
			}

			switch (gameSetup.st_game) {
			case G_INIT: gINIT(cfgSave.G2_ActivationDelay); break;
			case G_LGSD: gCfg_LGSD(cfgSave.G2_ActivationDelay); break;

			case G_ActivationDelay: gActivationDelay(); break; // Ожидание начало игры
			case G_ActivationBOMB: gActivationBOMB(cfgSave.G2_ActivationFlags,
													cfgSave.G2_Password,
													cfgSave.G2_BombTimer * 60); break; // Активация бомбы


			case G_BombTimer: g2BombTimer(); break; //Отсчет взрыва бомбы


			case G_END: gEnd(gameSetup.Win ? cfgSave.G2_ActivationFlags : cfgSave.G2_DeactivationFlags); break;

			case G_TIME_END: //нужно отрисовать и выйти
				Display::clear();
				Display::drawImage(HALF_DISP_W - img_s1i2.width / 2, HALF_DISP_H - img_s1i2.height / 2, &img_s1i2);
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
	Menu::run_s1i2();
	phyDev.Real_State = STATE_Ready;
}
