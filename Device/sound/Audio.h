/*
 * Audio.h
 *
 *  Created on: 26 апр. 2020 г.
 *      Author: Admin
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>

typedef struct {
	uint8_t id;
	uint16_t delay;
} SONG;

//+1 power_on	Эффект при включении питания устройства	Эффект
const uint8_t effectPower_on = 1;

//+2 switch_Game_mode	Эффект идет сразу после запуска питания - выберите сценарий игры	Эффект
const uint8_t effectAfterPower = 2;

//+3 switch_Game_mode_voice_ru	Голос "Выберите сценарий игры"	Голос русский
//4 switch_Game_mode_voice_eng	Select game scenario	Голос английский
const uint8_t voiceSwitchGameMode[] = {3, 4};

//+5 judgment_day_ru	Судный день	Голос русский
//6 judgment_day_eng	"Judgment Day"	Голос английский
const uint8_t voiceJudgmentDay[] = {5,6};

//+7 Dirty_bomb_ru	Грязная бомба	Голос русский
//8 Dirty_bomb_eng	"Dirty bomb"	Голос английский
const uint8_t voiceDirtyBomb[] = {7,8};

//+9 Secret_code_ru	Секретный код	Голос русский
//10 Secret_code_eng	"Secret code"	Голос английский
const uint8_t voiceSecretCode[] = {9,10};

//+11 click	Эффект, что то вроде короткого клацания клавиш	Эффект
const uint8_t effectClick = 11;

//+12 click_on	Эффект нажатие на кнопку Звездочка на клавиатуре	Эффект
const uint8_t effectClickOn = 12;

//+13 click_esc	Эффект нажатие на решетку на клавиатуре	Эффект
const uint8_t effectClickEsc = 13;

//+14 ticker	Тикер, звук отсчета времени в секундах	Эффект
const uint8_t effectTicker = 14;

//+15 number_correctly	Эффект - Правильно введенная цифра при активации паролем 	Эффект
const uint8_t effectNumberCorrectly = 15;

//+16 digit_incorrectly	Эффект - неправильно введенная цифра при активации паролем 	Эффект
const uint8_t effectDigitIncorrectly = 16;

//+17 explosion	Взрыв устройства в конце сценария 	Эффект
const uint8_t effectExplosion = 17;

//+18 round_completed	Голосом - "Раунд Завершен"	Эффект
const uint8_t effectRoundCompleted[] = {18, 35};

//+19 victory	Крутой эффект победы команды музыка на 5-15 сек	Эффект
const uint8_t effectVictory = 19;

//+20 insert_flash_drive	Эффект когда подключают флешку дешифратор в устройство	Эффект
const uint8_t effectInsertFlashDrive = 20;

//+21 insert_flash_drive_ru	"Вставьте флешку в устройство"	Голос русский
//22 insert_flash_drive_eng	"Insert the flash drive into the device"	Голос английский
const uint8_t voiceInsertFlashDrive[] = {21,22};

//+23 pull_flash_drive	Эффект когда вынимают флешку	Эффект
const uint8_t effectPullFlashDrive = 23;

//-----------------------------------24	Miss	Пролетающая пуля при стрельбе своей команды в чемоданчик	Эффект
const uint8_t effectMiss =  24;

//+25 Hit	Звук, когда выстрелили в ящик и попали	Эффект
const uint8_t effectHit = 25;

//+26 Confirm_action_shot_device	Подтвердите действие выстрелом в устройство	Эффект
const uint8_t effectConfirmActionShotDevice = 26;

//+27 Confirm_action_shot_device_voice_ru	Фраза голосом "Подтвердите действие выстрелом в устройство"	Голос русский
//28 Confirm_action_shot_device_voice_eng	Сonfirm the action with a shot to the device	Голос английский
const uint8_t voiceConfirmActionShotDevice[] = {27,28};

//+29 device_activated	Эффект какого-нибудь виу-виу, или шифра... что Устройство активировано	Эффект
const uint8_t effectDeviceActivated = 29;

//+30 device_activated_voice_ru	Голосом "Устройство активировано"	Голос русский
//31 device_activated_voice_eng	Device is activated"	Голос английский
const uint8_t voiceDeviceActivated[] = {30, 31};

//+32 device_deactivated	Устройство деактивировано 	Эффект
const uint8_t effectDeviceDeactivated = 32;

//+33 device_deactivated_ru	"Устройство деактивировано"	Голос русский
//34 device_deactivated_eng	"device is deactivated"	Голос английский
const uint8_t voiceDeviceDeactivated[] = {33,34};

const uint8_t voiceSettings[] = {36,37};

#endif /* AUDIO_H_ */
