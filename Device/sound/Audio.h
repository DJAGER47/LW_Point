/*
 * Audio.h
 *
 *  Created on: 26 ���. 2020 �.
 *      Author: Admin
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>

typedef struct {
	uint8_t id;
	uint16_t delay;
} SONG;

//+1 power_on	������ ��� ��������� ������� ����������	������
const uint8_t effectPower_on = 1;

//+2 switch_Game_mode	������ ���� ����� ����� ������� ������� - �������� �������� ����	������
const uint8_t effectAfterPower = 2;

//+3 switch_Game_mode_voice_ru	����� "�������� �������� ����"	����� �������
//4 switch_Game_mode_voice_eng	Select game scenario	����� ����������
const uint8_t voiceSwitchGameMode[] = {3, 4};

//+5 judgment_day_ru	������ ����	����� �������
//6 judgment_day_eng	"Judgment Day"	����� ����������
const uint8_t voiceJudgmentDay[] = {5,6};

//+7 Dirty_bomb_ru	������� �����	����� �������
//8 Dirty_bomb_eng	"Dirty bomb"	����� ����������
const uint8_t voiceDirtyBomb[] = {7,8};

//+9 Secret_code_ru	��������� ���	����� �������
//10 Secret_code_eng	"Secret code"	����� ����������
const uint8_t voiceSecretCode[] = {9,10};

//+11 click	������, ��� �� ����� ��������� �������� ������	������
const uint8_t effectClick = 11;

//+12 click_on	������ ������� �� ������ ��������� �� ����������	������
const uint8_t effectClickOn = 12;

//+13 click_esc	������ ������� �� ������� �� ����������	������
const uint8_t effectClickEsc = 13;

//+14 ticker	�����, ���� ������� ������� � ��������	������
const uint8_t effectTicker = 14;

//+15 number_correctly	������ - ��������� ��������� ����� ��� ��������� ������� 	������
const uint8_t effectNumberCorrectly = 15;

//+16 digit_incorrectly	������ - ����������� ��������� ����� ��� ��������� ������� 	������
const uint8_t effectDigitIncorrectly = 16;

//+17 explosion	����� ���������� � ����� �������� 	������
const uint8_t effectExplosion = 17;

//+18 round_completed	������� - "����� ��������"	������
const uint8_t effectRoundCompleted[] = {18, 35};

//+19 victory	������ ������ ������ ������� ������ �� 5-15 ���	������
const uint8_t effectVictory = 19;

//+20 insert_flash_drive	������ ����� ���������� ������ ���������� � ����������	������
const uint8_t effectInsertFlashDrive = 20;

//+21 insert_flash_drive_ru	"�������� ������ � ����������"	����� �������
//22 insert_flash_drive_eng	"Insert the flash drive into the device"	����� ����������
const uint8_t voiceInsertFlashDrive[] = {21,22};

//+23 pull_flash_drive	������ ����� �������� ������	������
const uint8_t effectPullFlashDrive = 23;

//-----------------------------------24	Miss	����������� ���� ��� �������� ����� ������� � ����������	������
const uint8_t effectMiss =  24;

//+25 Hit	����, ����� ���������� � ���� � ������	������
const uint8_t effectHit = 25;

//+26 Confirm_action_shot_device	����������� �������� ��������� � ����������	������
const uint8_t effectConfirmActionShotDevice = 26;

//+27 Confirm_action_shot_device_voice_ru	����� ������� "����������� �������� ��������� � ����������"	����� �������
//28 Confirm_action_shot_device_voice_eng	�onfirm the action with a shot to the device	����� ����������
const uint8_t voiceConfirmActionShotDevice[] = {27,28};

//+29 device_activated	������ ������-������ ���-���, ��� �����... ��� ���������� ������������	������
const uint8_t effectDeviceActivated = 29;

//+30 device_activated_voice_ru	������� "���������� ������������"	����� �������
//31 device_activated_voice_eng	Device is activated"	����� ����������
const uint8_t voiceDeviceActivated[] = {30, 31};

//+32 device_deactivated	���������� �������������� 	������
const uint8_t effectDeviceDeactivated = 32;

//+33 device_deactivated_ru	"���������� ��������������"	����� �������
//34 device_deactivated_eng	"device is deactivated"	����� ����������
const uint8_t voiceDeviceDeactivated[] = {33,34};

const uint8_t voiceSettings[] = {36,37};

#endif /* AUDIO_H_ */
