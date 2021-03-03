#include "Language.h"

const char* s_per[2] = {"%","%"};
const char* s_sec[2] = {"���","sec"};
const char* s_min[2] = {"���","min"};
const char* s_true[2] =  {"��������" , "ENABLE"};
const char* s_false[2] = {"���������", "DISABLE"};
const char* s_lang[2] = {"�������", "ENGLISH"};
const char* s_win[2] = {"������ ", "VICTORY "};
const char* s_FW[2] = {"��������", "FIRMWARE"};

const char* s_Exit[2] ={"�����" , "EXIT"};
const char* s_Yes[2] = {" �� " , " YES "};
const char* s_No[2] =  {" ��� ", " NO "};

const char ADFlags[] = {
		s_SHOT,
		s_PASS,
		s_USB
};

const char* langStgMenu[][2] = {
		{ "������ ����"  , "JUDGMENT DAY" }, //0
		{ "������� �����", "DIRTY BOMB" }, //1
		{ "��������� ���", "SECRET CODE" }, //2
		{ "���������"    , "SETTINGS" }, //3

		{ "���������" , "SYSTEM" }, //4
		{ "WIFI"      , "WIFI" }, //5
		{ "����������", "INFO" }  //6
};

const char* langStgGame[5][2] = {
		{"���������� �����"	,"ACTIVATION DELAY"},
		{"����� �� ���������","BOMB TIMER"},
		{"������"			, "PASSWORD"},
		{"���������"		,"ACTIVATION"},
		{"�����������"		,"DEACTIVATION"}
};

const char* langStgSystem[11][2] = {
		{"��������"				,"DEVICE NAME"},
		{"�������� ���������"	,"GAME START DELAY"},
		{"PLAYER ID"			,"PLAYER ID"},
		{"���������"			,"VOLUME"},
		{"����"					,"LANGUAGE"},
		{"IR ��������"			,"IR POWER"},
		{"IR PROTOCOL"			,"IR PROTOCOL"},
		{"����� ������"			,"TEAM MASK"},
		{"����� ������"			,"ROUND TIME"},
		{"��������� ������"		,"PULSE EXPLOSION"},
		{"�������"              ,"BRIGHTNESS"}
};

const char* langStgWifi[3][2] = {
		{"WIFI"		,"WIFI"},
		{"��������"	,"SIID"},
		{"����"		,"KEY"}
};

const char* langStgMaskTeam[][2] = {
		{"�������"	, "RED"},
		{"�����"	, "BLUE"},
		{"������"	, "YELLOW"},
		{"�������"	, "GREEN"},

		{"!MAGENTA", "MAGENTA"},
		{"!CYAN", "CYAN"},
		{"!BLACK", "BLACK(NOTHING)"},
		{"!WHITE", "WHITE(ALL)"}
};

const char* langGameStart[] = {"���� ��������...","GAME START..."};
const char* langGameBomb[] =  {"�� ������..."	 ,"TIME TO DETONATION"};
const char* langGameMOVE1[] = {"��������� �"     ,"MOVE TO"};
const char* langGameMOVE2[] = {"����� ����������","DESTINATION"};
const char* langGameMOVE3[] = {"� ������� *"     ,"AND PRESS *"};


