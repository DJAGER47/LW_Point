/*
 * version.c
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: lavrovskij
 */
#include "version.h"

bool changeConfig = false;
PhysicalDevice phyDev;
ConfigDevSave cfgSave;

const AT_VERSION at_version = {
		.GENERATION = 11,
		.MAJOR = 0,
		.MINOR = 0,
		.BUILD = 1
};

const ConfigDevSave cfgSaveDefault = {
//Configuration
		.Cfg_DeviceName = "\x05""Point",
		.Cfg_LocalGameStartDelay = 25, //0-65535
		.Cfg_PlayerID = 64, 	//1-65534
		.Cfg_Volume = 1,
		.Cfg_Language = 0,
		.Cfg_IRpower = 100,
		.Cfg_IRprotocol = 0,
		.Cfg_TeamMask = 255,	//1-255
		.Cfg_GameType = 0,
//Logical device 0
		.G1_DeviceID = 2,
		.G1_DeviceID = 10,
		.G1_ActivationDelay = 5,	//0-180
		.G1_BombTimer = 10,		// 1-999
		.G1_Password = "\x04""1234",
		.G1_ActivationFlags = FLAG_SHOT + FLAG_PASS + FLAG_FLASH,
		.G1_DeactivationFlags = 0,
//Logical device 1
		.G2_DeviceID = 2,
		.G2_DeviceID = 10,
		.G2_ActivationDelay = 5, //0-180
		.G2_BombTimer = 300, // 1-999
		.G2_Password = "\x04""1234",
		.G2_ActivationFlags   = FLAG_PASS + FLAG_SHOT,
		.G2_DeactivationFlags = FLAG_PASS + FLAG_SHOT,
//Logical device 2
		.G3_DeviceID = 2,
		.G3_DeviceID = 10,
		.G3_ActivationDelay = 5, //0-180
		.G3_BombTimer = 10, // 1-999
		.G3_Password = "\x04""1234",
		.G3_ActivationFlags = 0,
		.G3_DeactivationFlags = FLAG_SHOT + FLAG_PASS + FLAG_FLASH,
//Others save
		.wifi_en = true,
		.wifi_ssid = "Xiaomi_Wi-Fi",
		.wifi_key = "1357908642",

		.RoundTime = 600, //Время раунда
		.numPulseExplosion = 10, //Количество импульсов взрыва
		.brightnessX = 10
};

#define ITEM_COUNT 45
const T_ItemList CfgItems __attribute__((aligned(4))) = {
	ITEM_COUNT,
  {
	{ 0x01000005, .Type = 4, 0 }, //Слот master

		{ 0x01000301, .Type = 4, 0 }, //Физическое устройство
			//{ 0x03000004, .sType = { 4, 0, 0, 0 }, 1, 4 }, //Версия железа
			{ 0x03000003, .sType = { 4, 0, 0, 0 }, 2, 4 }, //Версия прошивки
			{ 0x03000005, .sType = { 1, 0, 0, 1 }, 3, 13 }, //Серийный номер

			{ 0x01000400, .Type = 9, 0 }, //Конфигурация
				{ 0x03000006, .sType = { 1, 0, 1, 1 }, 4, 32 }, //Имя устройства
				{ 0x04000004, .sType = { 2, 0, 1, 0 }, 5, 2 }, //Задержка старта локальной игры
				{ 0x04020001, .sType = { 2, 0, 1, 0 }, 6, 2 }, //Player ID
				{ 0x04030105, .sType = { 1, 0, 1, 0 }, 7, 1 }, //Громкость событий
				{ 0x04000005, .sType = { 1, 0, 1, 0 }, 8, 1 }, //Язык интерфейса
				{ 0x04030009, .sType = { 1, 0, 1, 0 }, 9, 1 }, //ИК мощность
				{ 0x04000001, .sType = { 1, 0, 1, 0 }, 10, 1 }, //IR protocol
				{ 0x04000002, .sType = { 1, 0, 1, 0 }, 11, 1 }, //Team mask
				{ 0x04050001, .sType = { 1, 0, 1, 0 }, 12, 1 }, //Game type

			{ 0x01000500, .Type = 3, 0 }, //Realtime
				{ 0x05000003, .sType = { 1, 0, 0, 0 }, 13, 1 }, //State
				{ 0x05000001, .sType = { 1, 0, 0, 0 }, 14, 1 }, //ACC level
				{ 0x00000003, .sType = { 1, 1, 0, 0 }, 15, 1 }, //Rssi

		{ 0x01000302, .Type = 0x02, 0 }, //Logical device 0
			{ 0x03000000, .sType = { 1, 0, 0, 0 }, 16, 1 }, //Device ID
			{ 0x01000401, .Type = 6, 0},
				{ 0x04000003, .sType = { 2, 0, 1, 0 }, 3, 0  }, //Длительность локальной игры
				{ 0x04050201, .sType = { 1, 0, 1, 0 }, 17, 1 }, //Activation delay
				{ 0x04050202, .sType = { 2, 0, 1, 0 }, 18, 2 }, //Bomb timer
				{ 0x04050203, .sType = { 1, 0, 1, 1 }, 19, 32 }, //Password
				{ 0x04050204, .sType = { 1, 0, 1, 0 }, 20, 1 }, //Activation flags
				{ 0x04050205, .sType = { 1, 0, 1, 0 }, 21, 1 }, //Deactivation flags

		{ 0x01000302, .Type = 0x02, 0 }, //Logical device 1
			{ 0x03000000, .sType = { 1, 0, 0, 0 }, 22, 1 }, //Device ID
			{ 0x01000401, .Type = 6, 0},
				{ 0x04000003, .sType = { 2, 0, 1, 0 }, 3, 0 }, //Длительность локальной игры
				{ 0x04050201, .sType = { 1, 0, 1, 0 }, 23, 1 }, //Activation delay
				{ 0x04050202, .sType = { 2, 0, 1, 0 }, 24, 2 }, //Bomb timer
				{ 0x04050203, .sType = { 1, 0, 1, 1 }, 25, 32 }, //Password
				{ 0x04050204, .sType = { 1, 0, 1, 0 }, 26, 1 }, //Activation flags
				{ 0x04050205, .sType = { 1, 0, 1, 0 }, 27, 1 }, //Deactivation flags

		{ 0x01000302, .Type = 0x02, 0 }, //Logical device 2
			{ 0x03000000, .sType = { 1, 0, 0, 0 }, 28, 1 }, //Device ID
			{ 0x01000401, .Type = 6, 0},
				{ 0x04000003, .sType = { 2, 0, 1, 0 }, 3, 0 }, //Длительность локальной игры
				{ 0x04050201, .sType = { 1, 0, 1, 0 }, 29, 1 }, //Activation delay
				{ 0x04050202, .sType = { 2, 0, 1, 0 }, 30, 2 }, //Bomb timer
				{ 0x04050203, .sType = { 1, 0, 1, 1 }, 31, 32 }, //Password
				{ 0x04050204, .sType = { 1, 0, 1, 0 }, 32, 1 }, //Activation flags
				{ 0x04050205, .sType = { 1, 0, 1, 0 }, 33, 1 }, //Deactivation flags
	}
};

const void* p_cfgDevSave[] = {
		0,
//Slot 0
//Physical device
		&phyDev.FWversion,
		phyDev.SN,
//Configuration
		cfgSave.Cfg_DeviceName,
		&cfgSave.Cfg_LocalGameStartDelay,
		&cfgSave.Cfg_PlayerID,
		&cfgSave.Cfg_Volume,
		&cfgSave.Cfg_Language,
		&cfgSave.Cfg_IRpower,
		&cfgSave.Cfg_IRprotocol,
		&cfgSave.Cfg_TeamMask,
		&cfgSave.Cfg_GameType,
//Realtime
		&phyDev.Real_State,
		&phyDev.Real_ACClevel,
		&phyDev.Real_Rssi,
//Logical device 0
		&cfgSave.G1_DeviceID,
		&cfgSave.G1_ActivationDelay,
		&cfgSave.G1_BombTimer,
		cfgSave.G1_Password,
		&cfgSave.G1_ActivationFlags,
		&cfgSave.G1_DeactivationFlags,
//Logical device 1
		&cfgSave.G2_DeviceID,
		&cfgSave.G2_ActivationDelay,
		&cfgSave.G2_BombTimer,
		cfgSave.G2_Password,
		&cfgSave.G2_ActivationFlags,
		&cfgSave.G2_DeactivationFlags,
//Logical device 2
		&cfgSave.G3_DeviceID,
		&cfgSave.G3_ActivationDelay,
		&cfgSave.G3_BombTimer,
		cfgSave.G3_Password,
		&cfgSave.G3_ActivationFlags,
		&cfgSave.G3_DeactivationFlags
};

void initPhyDev(){
	phyDev.FWversion = *((uint32_t*) ((AT_VERSION*) &at_version));
	phyDev.SN[0] = 0x0C;
	for (uint8_t i = 1; i < 13; i++)
		phyDev.SN[i] = STM32_UID8[i - 1];

	phyDev.Real_ACClevel = 0;
	phyDev.Real_State = STATE_PowerOn;
}


