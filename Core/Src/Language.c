#include "Language.h"

const char* s_per[2] = {"%","%"};
const char* s_sec[2] = {"ЯЕЙ","sec"};
const char* s_min[2] = {"ЛХМ","min"};
const char* s_true[2] =  {"бйкчвемн" , "ENABLE"};
const char* s_false[2] = {"бшйкчвемн", "DISABLE"};
const char* s_lang[2] = {"псяяйхи", "ENGLISH"};
const char* s_win[2] = {"онаедю ", "VICTORY "};
const char* s_FW[2] = {"опньхбйю", "FIRMWARE"};

const char* s_Exit[2] ={"бшунд" , "EXIT"};
const char* s_Yes[2] = {" дю " , " YES "};
const char* s_No[2] =  {" мер ", " NO "};

const char ADFlags[] = {
		s_SHOT,
		s_PASS,
		s_USB
};

const char* langStgMenu[][2] = {
		{ "ясдмши демэ"  , "JUDGMENT DAY" }, //0
		{ "цпъгмюъ анлаю", "DIRTY BOMB" }, //1
		{ "яейпермши йнд", "SECRET CODE" }, //2
		{ "мюярпнийх"    , "SETTINGS" }, //3

		{ "яхярелмше" , "SYSTEM" }, //4
		{ "WIFI"      , "WIFI" }, //5
		{ "хмтнплюжхъ", "INFO" }  //6
};

const char* langStgGame[5][2] = {
		{"нркнфеммши ярюпр"	,"ACTIVATION DELAY"},
		{"бпелъ дн дернмюжхх","BOMB TIMER"},
		{"оюпнкэ"			, "PASSWORD"},
		{"юйрхбюжхъ"		,"ACTIVATION"},
		{"деюйрхбюжхъ"		,"DEACTIVATION"}
};

const char* langStgSystem[11][2] = {
		{"мюгбюмхе"				,"DEVICE NAME"},
		{"гюдепфйю юйрхбюжхх"	,"GAME START DELAY"},
		{"PLAYER ID"			,"PLAYER ID"},
		{"цпнлйнярэ"			,"VOLUME"},
		{"ъгшй"					,"LANGUAGE"},
		{"IR лнымнярэ"			,"IR POWER"},
		{"IR PROTOCOL"			,"IR PROTOCOL"},
		{"люяйю йнлюмд"			,"TEAM MASK"},
		{"бпелъ пюсмдю"			,"ROUND TIME"},
		{"хлоскэянб бгпшбю"		,"PULSE EXPLOSION"},
		{"ъпйнярэ"              ,"BRIGHTNESS"}
};

const char* langStgWifi[3][2] = {
		{"WIFI"		,"WIFI"},
		{"мюгбюмхе"	,"SIID"},
		{"йкчв"		,"KEY"}
};

const char* langStgMaskTeam[][2] = {
		{"йпюямши"	, "RED"},
		{"яхмхи"	, "BLUE"},
		{"фекрши"	, "YELLOW"},
		{"гекемши"	, "GREEN"},

		{"!MAGENTA", "MAGENTA"},
		{"!CYAN", "CYAN"},
		{"!BLACK", "BLACK(NOTHING)"},
		{"!WHITE", "WHITE(ALL)"}
};

const char* langGameStart[] = {"хцпю мювмеряъ...","GAME START..."};
const char* langGameBomb[] =  {"дн бгпшбю..."	 ,"TIME TO DETONATION"};
const char* langGameMOVE1[] = {"днярюбэре б"     ,"MOVE TO"};
const char* langGameMOVE2[] = {"рнвйс мюгмювемхъ","DESTINATION"};
const char* langGameMOVE3[] = {"х мюфлхре *"     ,"AND PRESS *"};


