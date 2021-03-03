#include "menu.h"

extern SSD1283A myDisp;
extern osMessageQueueId_t myKeyHandle;

void run_s1i1();
void run_s1i2();
void run_s1i3();
void run_s1i4();

void startGame();

void stgGame1one();
void stgGame2one();
void stgGame3one();

void mSystemOne();
void mWifiOne();
void mInfoOne();

void stgSystemOne();
void stgWifiOne();
void stgInfoOne();


static menuItem* selectedMenuItem; // текущий пункт меню

#define MAKE_MENU(Name, Next, Previous, Parent, Child, one) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, one}

#define PREVIOUS   ((menuItem*)(selectedMenuItem->Previous))
#define NEXT       ((menuItem*)(selectedMenuItem->Next))
#define PARENT     ((menuItem*)(selectedMenuItem->Parent))
#define CHILD      ((menuItem*)(selectedMenuItem->Child))


#define NULL_ENTRY Null_Menu
const menuItem     Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0};

               //        NEXT,       PREV,    #PARENT,     *CHILD,
//       (   s1i1, NULL_ENTRY, NULL_ENTRY, NULL_ENTRY, NULL_ENTRY, run_s1i1, 0);
MAKE_MENU(   s1i1,       s1i2,       s1i4,    stg1Gm1, NULL_ENTRY, Menu::run_s1i1);
MAKE_MENU(   s1i2,       s1i3,       s1i1,    stg2Gm1, NULL_ENTRY, Menu::run_s1i2);
MAKE_MENU(   s1i3,       s1i4,       s1i2,    stg3Gm1, NULL_ENTRY, Menu::run_s1i3);
MAKE_MENU(   s1i4,       s1i1,       s1i3, NULL_ENTRY,    mSystem, run_s1i4); //Settings

//setting
MAKE_MENU(stg1Gm1, NULL_ENTRY, NULL_ENTRY,       s1i1, NULL_ENTRY, stgGame1one); //list
MAKE_MENU(stg2Gm1, NULL_ENTRY, NULL_ENTRY,       s1i2, NULL_ENTRY, stgGame2one); //list
MAKE_MENU(stg3Gm1, NULL_ENTRY, NULL_ENTRY,       s1i3, NULL_ENTRY, stgGame3one); //list

MAKE_MENU(mSystem,      mWiFi,      mINFO,       s1i4,     stgSys, mSystemOne); //Settings System
MAKE_MENU(mWiFi  ,      mINFO,    mSystem,       s1i4,    stgWIFI, mWifiOne);   //Settings WIFI
MAKE_MENU(mINFO  ,    mSystem,      mWiFi,       s1i4,    stgInfo, mInfoOne);   //Settings INFO

MAKE_MENU(stgSys , NULL_ENTRY, NULL_ENTRY,    mSystem, NULL_ENTRY, stgSystemOne); //list System
MAKE_MENU(stgWIFI, NULL_ENTRY, NULL_ENTRY,      mWiFi, NULL_ENTRY, stgWifiOne); //list WIFI
MAKE_MENU(stgInfo, NULL_ENTRY, NULL_ENTRY,      mINFO, NULL_ENTRY, stgInfoOne); //show info


//*MENU*GAME1************************************
static constexpr flagsGame flagADgame1 = {
		.count = { 6, 0 },
		._enum = {
				{ FLAG_PASS,
				  FLAG_PASS + FLAG_SHOT,
				  FLAG_FLASH,
				  FLAG_FLASH + FLAG_SHOT,
				  FLAG_PASS + FLAG_FLASH,
				  FLAG_SHOT + FLAG_PASS + FLAG_FLASH },
				{ }
		}
};
static constexpr paramMenuGame listMenuGame1 = {
	.flagAD = &flagADgame1,
	.len = 4,
	.l = {
		{ langStgGame[0], _u8,    &cfgSave.G1_ActivationDelay,  0, 180, s_sec},
		{ langStgGame[1], _u16,   &cfgSave.G1_BombTimer,        1, 99 , s_min},
		{ langStgGame[2], _123,   cfgSave.G1_Password ,         0, 0, 0},
		{ langStgGame[3], _Aenum, &cfgSave.G1_ActivationFlags,  0, 0, 0}
	  //{ langStgGame[4], _Denum, &cfgSave.G1_DeactivationFlags,0, 0}
	}
};
//*MENU*GAME2************************************
static constexpr flagsGame flagADgame2 = {
		.count = {2, 2},
		._enum = {
				{ FLAG_PASS,
				  FLAG_PASS + FLAG_SHOT},
				{ FLAG_PASS,
				  FLAG_PASS + FLAG_SHOT }
		}
};
static constexpr paramMenuGame listMenuGame2 = {
	.flagAD = &flagADgame2,
	.len = 5,
	.l = {
		{ langStgGame[0], _u8,    &cfgSave.G2_ActivationDelay,  0, 180, s_sec},
		{ langStgGame[1], _u16,   &cfgSave.G2_BombTimer,        1, 99 , s_min},
		{ langStgGame[2], _123,   cfgSave.G2_Password ,         0, 0, 0},
		{ langStgGame[3], _Aenum, &cfgSave.G2_ActivationFlags,  0, 0, 0},
	    { langStgGame[4], _Denum, &cfgSave.G2_DeactivationFlags,0, 0, 0}
	}
};
//*MENU*GAME3************************************
static constexpr flagsGame flagADgame3 = {
		.count = {0, 6},
		._enum = {
				{ 0 },
				{	FLAG_PASS,
					FLAG_PASS + FLAG_SHOT,
				  	FLAG_FLASH,
				  	FLAG_FLASH + FLAG_SHOT,
					FLAG_PASS + FLAG_FLASH,
					FLAG_SHOT + FLAG_PASS + FLAG_FLASH
				}
		}
};
static constexpr paramMenuGame listMenuGame3 = {
	.flagAD = &flagADgame3,
	.len = 3,
	.l = {
		{ langStgGame[0], _u8,    &cfgSave.G3_ActivationDelay,  0, 180, s_sec},
		//{ langStgGame[1], _u16,   &cfgSave.G3_BombTimer,        1, 999, s_sec},
		{ langStgGame[2], _123,   cfgSave.G3_Password ,         0, 0, 0},
		//{ langStgGame[3], _Aenum, &cfgSave.G3_ActivationFlags,  0, 0, 0},
		{ langStgGame[4], _Denum, &cfgSave.G3_DeactivationFlags,0, 0, 0}
	}
};
//***********************************************
void menuChange(menuItem* NewMenu){
	if ((void*)NewMenu == (void*)&NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
	if (selectedMenuItem->one)
		selectedMenuItem->one();
}

void Menu::startMenu() {
	switch (cfgSave.Cfg_GameType) {
		case 0: selectedMenuItem = &s1i1; break;
		case 1: selectedMenuItem = &s1i2; break;
		case 2: selectedMenuItem = &s1i3; break;
		default:selectedMenuItem = &s1i1; break;
	}

	if (selectedMenuItem->one)
		selectedMenuItem->one();
}
void Menu::menuHandler(){
	while (1){
		Key::eKEY key;
		osMessageQueueGet(myKeyHandle, &key, 0, HAL_MAX_DELAY);

		switch (key) {
		case Key::LEFT:
		case Key::RIGTH:Sound::urgentlySong(effectClick);   break;
		case Key::OK:	Sound::urgentlySong(effectClickOn); break;
		case Key::BACK:	Sound::urgentlySong(effectClickEsc);break;
		default: break;
		}

		switch (key){
		case Key::LEFT:		menuChange(NEXT); break;
		case Key::RIGTH:	menuChange(PREVIOUS); break;
		case Key::OK:
			if ((selectedMenuItem == &s1i1) || //start game
				(selectedMenuItem == &s1i2) ||
				(selectedMenuItem == &s1i3))
				return;
			menuChange(CHILD); break;
		case Key::BACK:		menuChange(PARENT); break;
		default: break;
		}
	}
}

//TOP MENU*******************************************************************
void Menu::run_s1i1(){
	gameSetup.LocalGameStartDelay = true;
	cfgSave.Cfg_GameType = 0;
	drawTopMenu(&img_s1i1, 0);
	Sound::addSong(voiceJudgmentDay[Lang]);
}
void Menu::run_s1i2(){
	gameSetup.LocalGameStartDelay = true;
	cfgSave.Cfg_GameType = 1;
	drawTopMenu(&img_s1i2, 1);
	Sound::addSong(voiceDirtyBomb[Lang]);
}
void Menu::run_s1i3(){
	gameSetup.LocalGameStartDelay = true;
	cfgSave.Cfg_GameType = 2;
	drawTopMenu(&img_s1i3, 2);
	Sound::addSong(voiceSecretCode[Lang]);
}
void run_s1i4(){
	cfgSave.Cfg_GameType = ~0;
	drawTopMenu(&img_s1i4, 3);
	Sound::addSong(voiceSettings[Lang]);
}

//SECOND MENU GAME*******************************************************************
void stgGame1one(){
	processSettings(listMenuGame1);
}
void stgGame2one(){
	processSettings(listMenuGame2);
}
void stgGame3one(){
	processSettings(listMenuGame3);
}


//SECOND MENU SETTING*******************************************************************
void mSystemOne(){
	drawTopMenu(&img_System, 4 );
}
void mWifiOne(){
	drawTopMenu(&img_wifi, 5 );
}
void mInfoOne(){
	drawTopMenu(&img_info, 6 );
}


static constexpr paramMenuGame listSystem = {
	.flagAD = 0,
	.len = 9,
	.l = {
		{langStgSystem[0], _nstr, &cfgSave.Cfg_DeviceName,          0,0,0},
		{langStgSystem[1], _u16,  &cfgSave.Cfg_LocalGameStartDelay, 0,65535, s_sec},
	  //{langStgSystem[2], _u16,  &cfgSave.Cfg_PlayerID,            1,65534, 0},
		{langStgSystem[3], _u8,   &cfgSave.Cfg_Volume,              0,100, s_per},
		{langStgSystem[4], _lang, &cfgSave.Cfg_Language,            0,1,0},
		{langStgSystem[5], _u8,   &cfgSave.Cfg_IRpower,             1,100, s_per},
	  //{langStgSystem[6], _u8,   &cfgSave.Cfg_IRprotocol,          0,1,0},
		{langStgSystem[7], _mask, &cfgSave.Cfg_TeamMask,            1,255,0},
		{langStgSystem[8], _rTime,&cfgSave.RoundTime,				5,180, s_min},
		{langStgSystem[9], _u8,   &cfgSave.numPulseExplosion,		1,50,0},
		{langStgSystem[10],_u8,   &cfgSave.brightnessX,             0,7,0}
	}
};

static constexpr paramMenuGame listWifi = {
	.flagAD = 0,
	.len = 3,
	.l = {
		{langStgWifi[0], _bool, &cfgSave.wifi_en,   0,0},
		{langStgWifi[1], _str,  &cfgSave.wifi_ssid, 0,0},
		{langStgWifi[2], _str,  &cfgSave.wifi_key,  0,0}
	}
};

void stgSystemOne(){
	processSettings(listSystem);
}
void stgWifiOne(){
	processSettings(listWifi);
}
void stgInfoOne(){
	myDisp.fillScreen(WHITE);
	//myDisp.:drawImage(0, 8, &img_MenuInfo);

	constexpr uint8_t xStart = 13;
	char str[32];
	myDisp.drawText(8, xStart, langStgSystem[0][Lang], Font_7x10);

	uint8_t len = cfgSave.Cfg_DeviceName[0];
	memcpy(str, cfgSave.Cfg_DeviceName + 1, len);
	str[len] = 0;
	uint8_t x = myDisp.drawChar(8, xStart + 11, s_angle, Font_7x10);
	myDisp.drawText(x, xStart + 11, str, Font_7x10);



	myDisp.drawText(8, xStart + 22, s_FW[Lang], Font_7x10);

	sprintf(str, "%d.%d.%d.%d",
			at_version.GENERATION,
			at_version.MAJOR,
			at_version.MINOR,
			at_version.BUILD);
	x = myDisp.drawChar(8, xStart + 33, s_angle, Font_7x10);
	myDisp.drawText(x, xStart + 33, str, Font_7x10);

	// Нихний бар
	myDisp.drawImage(DISP_W - 8, DISP_H - 8, &img_exit);
}
