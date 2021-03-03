/*
 * supportGame.c
 *
 *  Created on: 6 мая 2020 г.
 *      Author: Admin
 */

#include "game.h"

static uint32_t timer[5] = {0};
static uint32_t timer3game[3] = {0};


void zeroTimer(){
	memset(timer,0,sizeof(timer));
	memset(timer3game,0,sizeof(timer3game));
}

void updateTimeGame(uint32_t time){
	char str[10];
	uint8_t h = time / 3600;
	uint8_t m = time % 3600 / 60;
	uint8_t s = time % 60;
	sprintf(str, "%d%d:%d%d:%d%d", h / 10, h % 10, m / 10, m % 10, s / 10, s % 10);
	uint8_t x = Display::lenPixText(str, Font_5x7);
	Display::drawText(HALF_DISP_W - x/2, 0, str, Font_5x7, false);
}

//Введенный пароль пользователем
static char pass[32];
static uint8_t indexPass = 1;
void reinitPass(){
	memset(pass, 0, sizeof(pass));
	indexPass = 1;
	gameSetup.prevPass = true;
}

bool inputPass(char* chekPass){
	if (chekPass[indexPass] == 0)
		return true;

	if (!Key::isEmpty()){
		char k = Key::delKey();
		if (chekPass[indexPass] == k){
			pass[indexPass-1] = k;
			pass[indexPass] = 0;
			indexPass++;

			uint8_t len = Display::lenPixText(pass, Font_16x18);
			Display::drawClearRectangle(0, DISP_H - 20, 128, 20);
			Display::drawText(HALF_DISP_W - len / 2, DISP_H - 20, pass , Font_16x18);
			Sound::urgentlySong(effectNumberCorrectly);
		}else{
			Sound::urgentlySong(effectDigitIncorrectly);
		}
	}
	return false;
}

//Exit game
bool drawGameExit(){
	uint8_t prevVRAM[Display::RAMsize()];
	memcpy(prevVRAM, Display::RAM(), Display::RAMsize());

	constexpr uint8_t lenX = 60;
	constexpr uint8_t lenY = 35;
	constexpr uint8_t offsetY = 18;

	Display::drawRectangle(HALF_DISP_W - lenX/2, HALF_DISP_H - offsetY, lenX, lenY);

	uint8_t len = Display::lenPixText(s_Exit[Lang], Font_5x7);
	Display::drawText(HALF_DISP_W - len/2, HALF_DISP_H - offsetY + 1*7, s_Exit[Lang], Font_5x7, false, dRes, dSet);

	uint8_t yes = Display::lenPixText(s_Yes[Lang], Font_5x7);
	Display::drawText(HALF_DISP_W - lenX/4 - yes/2, HALF_DISP_H - offsetY + 3*7, s_Yes[Lang], Font_5x7, false, dRes, dSet); //Yes

	uint8_t no = Display::lenPixText(s_No[Lang], Font_5x7);
	Display::drawText(HALF_DISP_W + lenX/4 - no/2, HALF_DISP_H - offsetY + 3*7, s_No[Lang] , Font_5x7, false, dSet, dRes); //No

	Display::flush();
	uint32_t prevTime = SysTicks;
	bool exit = false;
	while (1) {
		game::update();
		if (!Key::isEmpty()) {
			char k = Key::delKey();

			if (Key::getShortPress()){
				switch(k){
				case key_OK:   Sound::addSong(effectClickOn); break;
				case key_Exit: Sound::addSong(effectClickEsc); break;
				default :      Sound::addSong(effectClick); break;
				}
			}

			switch (k){
			case key_Left:
				exit = true;
				Display::drawText(HALF_DISP_W - lenX/4 - yes/2, HALF_DISP_H - offsetY + 3*7, s_Yes[Lang], Font_5x7, false, dSet, dRes); //Yes
				Display::drawText(HALF_DISP_W + lenX/4 - no/2, HALF_DISP_H - offsetY + 3*7, s_No[Lang] , Font_5x7, false, dRes, dSet); //No
				break;
			case key_Right:
				exit = false;
				Display::drawText(HALF_DISP_W - lenX/4 - yes/2, HALF_DISP_H - offsetY + 3*7, s_Yes[Lang], Font_5x7, false, dRes, dSet); //Yes
				Display::drawText(HALF_DISP_W + lenX/4 - no/2, HALF_DISP_H - offsetY + 3*7, s_No[Lang] , Font_5x7, false, dSet, dRes); //No
				break;
			case key_OK:
				if (!exit){
					gameSetup.tStartGame += SysTicks - prevTime;
					memcpy(Display::RAM(), prevVRAM, Display::RAMsize());
					Display::flush();
				}else{
					rgbColor(LED_BLACK);
					Sound::Stop();
				}
				return exit;
			}
		}
	}
}

bool processGameExit() {
static uint8_t countExitKey = 0;
	if (!Key::isEmpty()) {
		if (Key::getKey() == key_Exit) {
			Key::delKey();
			if (Key::getShortPress())
				Sound::addSong(effectClickEsc);

			countExitKey++;
			if (countExitKey > 20){
				countExitKey = 0;
				Key::reinit();
				return drawGameExit();
			}
		} else {
			countExitKey = 0;
		}
	}
	return false;
}



bool processFLASH(){
	bool flagFlash = Adc::getUSB();
	if (gameSetup.prevUSB != flagFlash) {
		gameSetup.prevUSB = flagFlash;

		if (flagFlash) {
			Display::clearBot();
			reinitPass();
			Key::reinit();
			gameSetup.prevShot = true;
			Sound::urgentlySong(effectInsertFlashDrive);
			Receiver::reinitShot();
		} else {
			Display::clearBot();
			Display::drawImage(HALF_DISP_W - (img_usb.width / 2), HALF_DISP_H - (img_usb.height / 2), &img_usb, false);
			Sound::urgentlySong(effectPullFlashDrive);
			Sound::addSong(voiceInsertFlashDrive[Lang], 300);
			timer3game[0] = SysTicks + 30000;
		}
	}

	if ((!flagFlash) && (timer3game[0] < SysTicks)){
		Sound::addSong(voiceInsertFlashDrive[Lang]);
		timer3game[0] = SysTicks + 30000;
	}

	return flagFlash;
}
bool processPass(char* pass){
	bool flagPass = inputPass(pass);
	if (gameSetup.prevPass != flagPass) {
		gameSetup.prevPass = flagPass;

		if (flagPass){
			Display::clearBot();
			gameSetup.prevShot = true;
			Receiver::reinitShot();
		}else{
			Display::clearBot();
			Display::drawImage(HALF_DISP_W - (img_pass.width / 2), HALF_DISP_H - (img_pass.height / 2), &img_pass, false);
		}
	}
	return flagPass;
}
bool processShot(){
	if (gameSetup.prevShot){
		Display::drawImage(HALF_DISP_W - (img_shot.width / 2), HALF_DISP_H - (img_shot.height / 2), &img_shot, false);
		Sound::urgentlySong(effectConfirmActionShotDevice);
		Sound::addSong(voiceConfirmActionShotDevice[Lang]);
		timer3game[2] = SysTicks + 30000;
		gameSetup.prevShot = false;
	}

	if (Receiver::isEmptyShot()){
		if (timer3game[2] < SysTicks){
			Sound::addSong(voiceConfirmActionShotDevice[Lang]);
			timer3game[2] = SysTicks + 30000;
		}

		return false;
	}else{
		IRR_DATA receiv = Receiver::getDataShot();

		if (!(cfgSave.Cfg_TeamMask & (1 << receiv.shot.color))) return false;

		gameSetup.shotActivate = receiv;
		Sound::urgentlySong(effectHit);
		return true;
	}
}

#if (0)
void ExplosionShot(uint16_t& countTemp){
	if (!countTemp) return;

	if (SysTicks < timer[0]) return;
		timer[0] = SysTicks + 500;

	IRR_DATA shot = gameSetup.shotActivate;
	shot.shot.damage = 0xF;
	if (!Transmitter::busy()){
		Transmitter::send(shot);
		countTemp--;
	}
}
#else
void ExplosionShot(uint16_t& countTemp){
	if (countTemp){
		Receiver::disable();

		if (SysTicks < timer[0])
			return;
		timer[0] = SysTicks + 500;

		IRR_DATA command;
		command.cmd.byte1 = irCMD;
		command.cmd.byte2 = irExplodePlayer;
		command.cmd.byte3 = irCMDEndByte3;
		if (!Transmitter::busy()) {
			Transmitter::send(command);
			countTemp--;
		}
	} else
		Receiver::enable();
}
#endif



void AnimS7Bomb(char c){
	if (SysTicks < timer[1]) return;
		timer[1] = SysTicks + 500;

	static bool reverse = true;
	reverse = !reverse;

	if (reverse){
		STLED316S::clear();
	}else{
		if (c == '0')
			all_str[0] = C7_0;
		else
			all_str[0] = C7_MIN;

		all_str[1] = all_str[2] = all_str[3] = all_str[0];
		STLED316S::writeData(all_str);
	}
}

void AnimS7Wait(){
	if (SysTicks > timer[2]) {
		timer[2] = SysTicks + 100;
		static uint8_t index = 0;
		if (index >= sizeof(S7_GIF)) index = 0;
		all_str[0] = S7_GIF[index++];
		all_str[1] = all_str[2] = all_str[3] = all_str[0];
		STLED316S::writeData(all_str);
	}
}

bool AnimS7ReversTime(uint16_t &time){
bool flag = false;
	if (SysTicks > timer[3]){
		timer[3] = SysTicks + 1000;

		if (time){
			uint32_t t = (time / 60) * 100 +
						 (time % 60);
			STLED316S::setNumber(t);
			time--;
		}else
			flag = true;
	}
	return flag;
}

void rgbColor(LED_COLOR color){
static bool _t = true;
	if (SysTicks > timer[4]){
		if (_t){
			timer[4] = SysTicks + 1000;
			RGB::setColor(color);
		}else{
			timer[4] = SysTicks + 800;
			RGB::setColor(LED_BLACK);
		}
		_t = !_t;
	}
}

//------GAME ST------------------------------------------
void displayLocalGameStartDelay(){
	Display::clearBot();
	uint8_t x = HALF_DISP_W - Display::lenPixText(langGameMOVE1[Lang], Font_7x10)/2;
	Display::drawText(x, offsetTextY(1), langGameMOVE1[Lang], Font_7x10);
	x = HALF_DISP_W - Display::lenPixText(langGameMOVE2[Lang], Font_7x10)/2;
	Display::drawText(x, offsetTextY(2), langGameMOVE2[Lang], Font_7x10);
	x = HALF_DISP_W - Display::lenPixText(langGameMOVE3[Lang], Font_7x10)/2;
	Display::drawText(x, offsetTextY(3), langGameMOVE3[Lang], Font_7x10);
}


void gINIT(uint8_t ActivationDelay) { // Начальный экран
	Disk::saveCfg();

	gameSetup.delayTimer = SysTicks;
	if (gameSetup.LocalGameStartDelay){
		Display::clearBot();
		uint8_t x = Display::lenPixText(langGameStart[Lang], Font_7x10);
		Display::drawText(HALF_DISP_W - x/2, HALF_DISP_H - 5, langGameStart[Lang], Font_7x10);
		gameSetup.countTemp = cfgSave.Cfg_LocalGameStartDelay;

		gameSetup.st_game = G_LGSD;
		phyDev.Real_State = STATE_WaitStart;
	}else{
		displayLocalGameStartDelay();
		gameSetup.countTemp = ActivationDelay;
		gameSetup.tStartGame = SysTicks; //игра началась

		gameSetup.st_game = G_ActivationDelay;
		phyDev.Real_State = STATE_RunGame;
	}
}

void gCfg_LGSD(uint8_t ActivationDelay){ //LocalGameStartDelay
	if (SysTicks < gameSetup.delayTimer) return;
		gameSetup.delayTimer = SysTicks + 1000;

	if (gameSetup.countTemp){
		char str[10];
		uint8_t m = gameSetup.countTemp / 60;
		uint8_t s = gameSetup.countTemp % 60;
		sprintf(str, "%d%d:%d%d",  m / 10, m % 10, s / 10, s % 10);
		uint8_t x = Display::lenPixText(str, Font_16x18);
		Display::drawClearRectangle(HALF_DISP_W - x/2, DISP_H - 20, x, 20);
		Display::drawText(HALF_DISP_W - x/2, DISP_H - 20, str, Font_16x18, false);
		gameSetup.countTemp--;
		return;
	}

	displayLocalGameStartDelay();

	gameSetup.tStartGame = SysTicks; //игра началась
	gameSetup.countTemp = ActivationDelay;
	gameSetup.delayTimer = SysTicks;
	gameSetup.st_game = G_ActivationDelay;
	phyDev.Real_State = STATE_RunGame;
}

void gActivationDelay() { // Задержка старта
	if (!gameSetup.startStar){
		if (!AnimS7ReversTime(gameSetup.countTemp)) return;
		all_str[0] = all_str[1] = all_str[2] = all_str[3] = C7_MIN;
		STLED316S::writeData(all_str);
		gameSetup.startStar = true;
		return;
	}

	if (Key::isEmpty()) return;
	if (Key::getKey() != key_OK) return;
	Sound::addSong(effectClickOn);


	Display::clearBot();
	Receiver::reinitShot();
	gameSetup.st_game = G_ActivationBOMB;
}

void gActivationBOMB(uint8_t ActivationFlags, char* Password, uint16_t BombTimer) { // Ожидание активации бомбы
	AnimS7Wait();

	if (ActivationFlags & FLAG_FLASH)
		if (!processFLASH()) return;

	if (ActivationFlags & FLAG_PASS)
		if (!processPass(Password)) return;

	if (ActivationFlags & FLAG_SHOT)
		if (!processShot()) return;

	//EXIT
	reinitPass();

	Sound::addSong(effectDeviceActivated);
	Sound::addSong(voiceDeviceActivated[Lang]);

	if (ActivationFlags & FLAG_SHOT)
		RGB::setColor((LED_COLOR)gameSetup.shotActivate.shot.color);
	else
		RGB::setColor(LED_WHITE);

	Display::clearBot();
	uint8_t x = Display::lenPixText(langGameBomb[Lang], Font_7x10);
	Display::drawText(HALF_DISP_W - x/2, HALF_DISP_H, langGameBomb[Lang], Font_7x10);

	gameSetup.countTemp = BombTimer;
	gameSetup.delayTimer = SysTicks;
	gameSetup.st_game = G_BombTimer;
}



void gEnd(uint8_t ActivationFlags){
	ExplosionShot(gameSetup.countTemp);
	AnimS7Bomb('0');

	if (ActivationFlags & FLAG_SHOT)
		rgbColor((LED_COLOR)gameSetup.shotActivate.shot.color);
	else
		rgbColor(LED_WHITE);

	if (SysTicks < gameSetup.delayTimer) return;
	gameSetup.delayTimer = SysTicks + 2000;

	if (ActivationFlags & FLAG_SHOT)
		gameSetup.showWin = !gameSetup.showWin;

	if (gameSetup.showWin){
		Display::drawImageFull(gameSetup.imgEnd);
	}else{
		Display::clear();
		char str[20];
		strcpy(str, s_win[Lang]);
		uint8_t len = strlen(s_win[Lang]);
		strcpy(str + len, langStgMaskTeam[gameSetup.shotActivate.shot.color][Lang]);

		len = Display::lenPixText(str, Font_7x10);
		Display::drawText(HALF_DISP_W - len/2, offsetTextY(1), str, Font_7x10);

		auto id = gameSetup.shotActivate.shot.ID;
		sprintf(str, "ID: %d", id);
		len = Display::lenPixText(str, Font_7x10);
		Display::drawText(HALF_DISP_W - len/2, offsetTextY(2), str, Font_7x10);
	}
}
