/*
 * Секретный код
 *
 *  Created on: 9 мар. 2020 г.
 *      Author: Admin
 */
#include "game.h"


void g3INIT(uint8_t ActivationDelay) { // Начальный экран
	Disk::saveCfg();

	Display::clearBot();
	uint8_t x = Display::lenPixText(langGameStart[Lang], Font_7x10);
	Display::drawText(HALF_DISP_W - x / 2, HALF_DISP_H - 5, langGameStart[Lang], Font_7x10);
	gameSetup.delayTimer = SysTicks;
	if (gameSetup.LocalGameStartDelay) {
		gameSetup.countTemp = cfgSave.Cfg_LocalGameStartDelay;

		gameSetup.st_game = G_LGSD;
		phyDev.Real_State = STATE_WaitStart;
	} else {
		Sound::urgentlySong(effectDeviceActivated);
		Sound::addSong(voiceDeviceActivated[Lang]);
		gameSetup.countTemp = ActivationDelay;
		gameSetup.tStartGame = SysTicks; //игра началась

		gameSetup.st_game = G_ActivationDelay;
		phyDev.Real_State = STATE_RunGame;
	}
}

void g3Cfg_LGSD(uint8_t ActivationDelay) { //LocalGameStartDelay
	if (SysTicks < gameSetup.delayTimer) return;
	gameSetup.delayTimer = SysTicks + 1000;

	if (gameSetup.countTemp) {
		char str[10];
		uint8_t m = gameSetup.countTemp / 60;
		uint8_t s = gameSetup.countTemp % 60;
		sprintf(str, "%d%d:%d%d", m / 10, m % 10, s / 10, s % 10);
		uint8_t x = Display::lenPixText(str, Font_16x18);
		Display::drawClearRectangle(HALF_DISP_W - x / 2, DISP_H - 20, x, 20);
		Display::drawText(HALF_DISP_W - x / 2, DISP_H - 20, str, Font_16x18,
				false);
		gameSetup.countTemp--;
		return;
	}

	Sound::urgentlySong(effectDeviceActivated);
	Sound::addSong(voiceDeviceActivated[Lang]);

	RGB::setColor(LED_WHITE);
	Display::clearBot();
	gameSetup.tStartGame = SysTicks; //игра началась
	gameSetup.countTemp = ActivationDelay;
	gameSetup.delayTimer = SysTicks;
	gameSetup.st_game = G_ActivationDelay;
	phyDev.Real_State = STATE_RunGame;
}

void g3DeactivationBOMB(uint8_t ActivationFlags, char* Password, uint16_t BombTimer) { // Ожидание активации бомбы
	AnimS7Wait();

	if (ActivationFlags & FLAG_FLASH)
		if (!processFLASH()) return;

	if (ActivationFlags & FLAG_PASS)
		if (!processPass(Password)) return;

	if (ActivationFlags & FLAG_SHOT)
		if (!processShot()) return;

	//EXIT
	reinitPass();

	Sound::urgentlySong(effectDeviceDeactivated);
	Sound::addSong(voiceDeviceDeactivated[Lang]);
	Sound::addSong(effectRoundCompleted[Lang]);
	Sound::addSong(effectVictory);

	STLED316S::clear();
	gameSetup.delayTimer = SysTicks;
	gameSetup.countTemp = 0;
	gameSetup.Win = true;
	gameSetup.imgEnd = &fimg_game3Win;
	gameSetup.st_game = G_END;
}

void g3BombTimer() {
	gameSetup.st_game = G_END;
}





void game::process_game3() {
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
				if (time == 0) gameSetup.st_game = G_TIME_END;
			}

			switch (gameSetup.st_game) {
				case G_INIT: g3INIT(cfgSave.G3_ActivationDelay); break;
				case G_LGSD: g3Cfg_LGSD(cfgSave.G3_ActivationDelay); break;

			//GAME
			case G_ActivationDelay: gActivationDelay(); break; // Задержка активации
			case G_ActivationBOMB: g3DeactivationBOMB(cfgSave.G3_DeactivationFlags,
												   	   cfgSave.G3_Password,
													   0); break; // Активация бомбы
			case G_BombTimer: g3BombTimer(); break; //Отсчет взрыва бомбы
			//END GAME

				case G_END: gEnd(cfgSave.G3_DeactivationFlags); break;

				case G_TIME_END: //нужно отрисовать и выйти
					Display::drawImageFull(&fimg_game3Losing);
					Sound::urgentlySong(effectRoundCompleted[Lang]);
					gameSetup.st_game = G_STOP;
					gameSetup.countTemp = cfgSave.numPulseExplosion;
					break;
				case G_STOP:
					AnimS7Bomb('-');
					rgbColor(LED_WHITE);
					ExplosionShot(gameSetup.countTemp);
					break;
			}
		}
		Key::reinit();
	}
	STLED316S::clear();
	Menu::run_s1i3();
	phyDev.Real_State = STATE_Ready;
}

