/*
 * version.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef VERSION_H_
#define VERSION_H_

#include <stdbool.h>
#include <stdint.h>

#define STM32_UID32 ((uint32_t *)0x1FFFF7E8)
#define STM32_UID8  ((uint8_t *)0x1FFFF7E8)
struct Uid {
    uint16_t X;  // x-coordinate
    uint16_t Y;  // y-coordinate
    uint8_t WAF;  // Wafer number
    char LOT[7];  // Lot number
};

typedef struct {
	uint8_t GENERATION;
	uint8_t MAJOR;
	uint8_t MINOR;
	uint8_t BUILD;
}AT_VERSION;

typedef struct {
	uint32_t ID;
	union {
		struct {
			uint8_t Size :5;
			uint8_t Sign :1;
			uint8_t Writeable :1;
			uint8_t Array :1;
		}sType;
		uint8_t Type;
	};
	uint8_t index;
	uint16_t size;
} T_ItemInfo;

typedef struct {
	uint32_t ItemCount;
	T_ItemInfo List[];
} T_ItemList;

typedef struct {
	uint32_t FWversion;
	char 	 SN[13];

	//Realtime
	uint8_t  Real_State;
	uint8_t  Real_ACClevel;
	int8_t   Real_Rssi;

	//others
	bool wifi_server_initOk;
}PhysicalDevice;

typedef struct {
//Configuration
char 	 Cfg_DeviceName[32];
uint16_t Cfg_LocalGameStartDelay;
uint16_t Cfg_PlayerID;
uint8_t  Cfg_Volume;
uint8_t  Cfg_Language;
uint8_t  Cfg_IRpower;
uint8_t  Cfg_IRprotocol;
uint8_t  Cfg_TeamMask;
uint8_t  Cfg_GameType;//Номер последней игры 0-2

//Logical device 0
uint8_t  G1_DeviceID;
uint8_t  G1_ActivationDelay;
uint16_t G1_BombTimer;
char 	 G1_Password[32];
uint8_t  G1_ActivationFlags;
uint8_t  G1_DeactivationFlags;

//Logical device 1
uint8_t  G2_DeviceID;
uint8_t  G2_ActivationDelay;
uint16_t G2_BombTimer;
char 	 G2_Password[32];
uint8_t  G2_ActivationFlags;
uint8_t  G2_DeactivationFlags;

//Logical device 2
uint8_t  G3_DeviceID;
uint8_t  G3_ActivationDelay;
uint16_t G3_BombTimer;
char 	 G3_Password[32];
uint8_t  G3_ActivationFlags;
uint8_t  G3_DeactivationFlags;

//others save
bool    wifi_en;
uint8_t wifi_ssid[32];
uint8_t wifi_key[32];


uint32_t RoundTime; //Время раунда
uint8_t numPulseExplosion; //Количество импульсов взрыва
uint8_t brightnessX; // максимальная яркость посветки RGB

uint32_t crc;
} ConfigDevSave;

enum FLAG{
	FLAG_SHOT = 1,
	FLAG_PASS = 2,
	FLAG_FLASH = 4
};

enum STATE{
	STATE_PowerOn = 0,
	STATE_Ready = 1,
	STATE_WaitStart = 2,
	STATE_RunGame = 3,
	STATE_End = 4
};

enum TeamMask{
	TeamMask_Red = 1,
	TeamMask_Blue = 2,
	TeamMask_Yellow = 4,
	TeamMask_Green = 8,
	TeamMask_Magenta = 16,
	TeamMask_Cyan = 32,
	TeamMask_Black_nothing = 64,
	TeamMask_White_all = 128
};

extern bool changeConfig;
extern const AT_VERSION at_version;
extern const T_ItemList CfgItems;
extern ConfigDevSave cfgSave;
extern const ConfigDevSave cfgSaveDefault;
extern PhysicalDevice phyDev;
extern const void* p_cfgDevSave[];
#define Lang cfgSave.Cfg_Language

#ifdef __cplusplus
 extern "C" {
#endif

void initPhyDev();

#ifdef __cplusplus
}
#endif

#endif /* VERSION_H_ */
