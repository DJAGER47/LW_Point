/*
 * game.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef GAME_H_
#define GAME_H_

#include "define.h"
#include "version.h"
#include "Language.h"
#include "menu.h"
#include "bmp.h"

#include "../Device/disk/Disk.h"
#include "../Device/WiFi/wifi_protocol.h"
#include "../Device/keyboard/key.h"
#include "../Device/IR/receiver.h"
#include "../Device/IR/transmitter.h"
#include "../Device/ssd1283A/ssd1283A.h"
#include "../Device/sound/Sound.h"
#include "../Device/RGBRing/RGBRing.h"
#include "../Device/RGB/RGB.h"

typedef enum{
	G_INIT = 0, 			// Начальный экран
	G_LGSD, 				//LocalGameStartDelay
	G_ActivationDelay,		// Задержка старта
	G_ActivationBOMB,		// Ожидание активации бомбы
	G_BombTimer,
	G_END,


	G_TIME_END,
	G_STOP
}ST_GAME;

typedef struct{
	bool LocalGameStartDelay; //Есть задержка перед игрой или нет
	bool isExit; //Флаг выхода из игры
	bool isPause; //Флаг паузы
	uint32_t timePause; //Время, когда поставили на паузу
	bool prevUSB; //Предыдущее значение наличия USB
	bool prevPass;
	bool prevShot;
	bool showWin;
	bool startStar; //Бамба на месте, играем, после нажатия *

	bool Win;
	const tImage* imgEnd;


	uint8_t SrvCtrl; //Флаг серверного управления

	//Start game
	int32_t tTimeStartGame; //Когда начнеться игра //TimeLeft Время до старта игровой сессии в мсек
	uint32_t tStartGame; //Время начала игры    //RoundTime Время раунда в секундах

	IRR_DATA shotActivate;   //Выстрел активированный бомбу


	ST_GAME st_game = G_INIT;
	uint16_t countTemp; //Отсчет старта, начала, бомбы, взрывы
	uint32_t delayTimer;

}GAME_SETUP;



void zeroTimer();
void updateTimeGame(uint32_t time);

void reinitPass();
bool inputPass(char* chekPass);

bool drawGameExit();
bool processGameExit();

bool processFLASH();
bool processPass(char* pass);
bool processShot();

void ExplosionShot(uint16_t& countTemp);
void AnimS7Bomb(char c);
void AnimS7Wait();
bool AnimS7ReversTime(uint16_t &time);
//void rgbColor(LED_COLOR color);


void gINIT(uint8_t);
void gCfg_LGSD(uint8_t);
void gActivationDelay();
void gActivationBOMB(uint8_t, char*, uint16_t);


void gEnd(uint8_t ActivationFlags);


class game {
private:
	static void process_game1();
	static void process_game2();
	static void process_game3();

public:
	static void handler();
};

extern STATUS_RECEIV status_receive_wifi;
extern GAME_SETUP gameSetup;

#endif /* GAME_H_ */
