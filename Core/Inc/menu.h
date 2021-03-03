#ifndef _MENU_H_
#define _MENU_H_

#include <stdint.h>
#include "bmp.h"
#include "game.h"
#include "version.h"
#include "Language.h"

#define sKey_space 0
#define sKey_back  1
#define sKey_shift 2
#define sKey_lang  3
#define sKey_num   4
#define sKey_okay  5

typedef const struct {
	void *Next;
	void *Previous;
	void *Parent;
	void *Child;
	_func one;
} menuItem;

enum {
	_Aenum = 0, //активация
	_Denum = 1, //деактивация
	_u8,
	_u16,
	_rTime, //round time
	_123, //password
	_str,
	_nstr, //неправильный string
	_bool,
	_lang,
	_mask //Маска команд
};

typedef enum{
	eNONE = 0,
	eSIMPLE,
	eSTRING,
	eCHECKBOX
}menuENUM;


typedef struct {
	const char** name;
	uint8_t type;
	void *data;
	uint8_t min;
	uint16_t max;
	const char** pref;
} paramMenu;

typedef struct{
	uint8_t count[2];
	uint8_t _enum[2][8];
}flagsGame;

typedef struct{
	const flagsGame* flagAD;
	uint8_t          len;
	paramMenu        l[];
}paramMenuGame;


void drawTopMenu(const tImage *img, uint8_t index);
void drawMenu(uint8_t num, const paramMenuGame &list);
void processSettings(const paramMenuGame &list);

void firstChangeStr(char* changeStr);
uint8_t changeStr(char k, char* changeStr);

void firstCheckbox(uint16_t& mask);
uint8_t changeCheckbox(char k, uint16_t& mask);

class Menu{
public:
	static void startMenu();
	static void menuHandler();

	static void run_s1i1();
	static void run_s1i2();
	static void run_s1i3();
};

#endif
