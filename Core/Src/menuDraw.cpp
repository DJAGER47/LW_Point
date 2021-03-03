/*
 * menuDraw.cpp
 *
 *  Created on: 20 апр. 2020 г.
 *      Author: Admin
 */

#include "menu.h"
#include "../Device/ssd1283A/ssd1283A.h"
#include "../Device/WiFi/wifi.h"

extern SSD1283A myDisp;
extern osMessageQueueId_t myKeyHandle;


const uint8_t cbMax = 4;

static uint16_t temporary;
static char temporaryArr[18];

void drawTopMenu(const tImage* img, uint8_t index){ //Главное меню
	myDisp.fillScreen(WHITE);
	// Центральная картинка
	myDisp.drawImage(HALF_DISP_W - img->width / 2, HALF_DISP_H - img->height / 2, img);
	// Стрелки
	myDisp.drawImage(20, HALF_DISP_H - img_left.height / 2, &img_left);
	myDisp.drawImage(DISP_W - 20 - img_right.width, HALF_DISP_H - img_right.height / 2, &img_right);
	// Нихний бар
	myDisp.drawImage(0, DISP_H - 10, &img_play);

	uint8_t x = myDisp.lenPixText(langStgMenu[index][Lang], Font_7x10);
	myDisp.drawText(HALF_DISP_W - x/2, DISP_H - 10, langStgMenu[index][Lang], Font_7x10);
	if (index < 3) myDisp.drawImage(DISP_W - 8, DISP_H - 10, &img_stg);
	else if (index > 3) myDisp.drawImage(DISP_W - 8, DISP_H - 10, &img_return);
}

void drawEnumValue(char* str, uint8_t data){
	char *p = str;
	*p++ = '<';
	for (auto i = 0; i < 3; i++) {
		if (data & (1 << i)) {
			*p++ = ADFlags[i];
			*p++ = '+';
		}
	}
	p--;
	*p++ = '>';
	*p = 0;
}
void drawParamValue(uint8_t ytext, uint8_t type, void* data, const char** pref, bool Set){
	uint16_t s, ns;
	if (Set){
		ns = WHITE;
		s = BLACK;
	}else{
		ns = BLACK;
		s = WHITE;
	}

	char str[18];
	str[0] = s_angle;
	char* str2 = &(str[1]);
	char* p;

	switch (type) {
	case _u8:
	case _u16:
		p = str2;
		if (type == _u8)
			p += sprintf(str2, "%d ", *((uint8_t*) data));
		else
			p += sprintf(str2, "%d ", *((uint16_t*) data));
		if (pref != 0) strcpy(p, pref[Lang]);
	break;

	case _rTime:{
		p = str2;
		uint32_t time = *((uint32_t*) data);
		p += sprintf(str2, "%d ", (uint8_t)(time/60));
		if (pref != 0) strcpy(p, pref[Lang]);
	}break;

	case _nstr:
	case _123:{
		p = (char*) data;
		uint8_t cnt = *((uint8_t*) p++);
		memcpy(str2, const_cast<const char*>(p), cnt);
		str2[cnt] = 0;
	}break;

	case _Aenum:
	case _Denum: drawEnumValue(str2, *((uint8_t*)data)); break;

	case _str:  strcpy(str2, const_cast<const char*>((char*)data)); break;
	case _lang: strcpy(str2, s_lang[*((uint8_t*) data)]); break;

	case _bool:
		if (*((bool*)data))
			strcpy(str2, s_true[Lang]);
		else
			strcpy(str2, s_false[Lang]);
		break;

	case _mask:{
		uint8_t index = 0;
		for (uint8_t i = 0; i < cbMax; i++){
			if (cfgSave.Cfg_TeamMask & (1<<i)){
				str2[index++] = *langStgMaskTeam[i][Lang];
				str2[index++] = '+';
			}
		}
		if (index > 0) str2[--index] = 0;
		else str2[index] = 0;
		}break;
	}

	myDisp.drawText(0, ytext, const_cast<const char*>(str), Font_7x10, false, s, ns);
}
void drawMenu(uint8_t num, const paramMenuGame& list){
	myDisp.fillScreen(WHITE);

	if (num)
		myDisp.drawImage(HALF_DISP_W - img_top.width / 2, 0, &img_top);
	if ((num + 3) < (list.len - 1))
		myDisp.drawImage(HALF_DISP_W - img_bottom.width / 2, DISP_H - 8, &img_bottom);

	myDisp.drawImage(0, DISP_H - 8, &img_ok);
	myDisp.drawImage(DISP_W - 8, DISP_H - 8, &img_return);

	//-----------------------------------------------
	uint8_t y = offsetTextY(0);
	const char* str = list.l[num].name[Lang];
	myDisp.drawRectangle(0, offsetItemY(0), DISP_W, hightY, BLACK);
	myDisp.drawText(0, y, str, Font_7x10, true, WHITE, BLACK);
	drawParamValue(offsetTextY(1), list.l[num].type, list.l[num].data, list.l[num].pref, true);

	//-----------------------------------------------
	for (uint8_t i = 1; i < 4; i++){
		if ((num + i) > (list.len - 1)) break;

		y = offsetTextY((2 * i));
		str = list.l[num + i].name[Lang];
		myDisp.drawText(0, y, str, Font_7x10, false);
		drawParamValue(offsetTextY(((2 * i) + 1)), list.l[num + i].type, list.l[num + i].data, list.l[num + i].pref, true);
	}
}

void editParamValue(uint8_t num, const paramMenuGame& list){
	Key::eKEY key;
	while(1){
		osMessageQueueGet(myKeyHandle, &key, 0, HAL_MAX_DELAY);

		switch (key) {
		case Key::LEFT:
		case Key::RIGTH:Sound::urgentlySong(effectClick);   break;
		case Key::OK:	Sound::urgentlySong(effectClickOn); break;
		case Key::BACK:	Sound::urgentlySong(effectClickEsc);break;
		default: break;
		}

		uint8_t type = list.l[num].type;


		switch (type) {
		case _rTime:
		case _u8:
		case _u16:
			if (key == Key::RIGTH) {
				temporary++;
				if (temporary > list.l[num].max)
					temporary = list.l[num].min;

			} else if (key == Key::LEFT) {
				temporary--;
				if ((temporary < list.l[num].min) || (temporary > list.l[num].max))
					temporary = list.l[num].max;
			} else
				break;

			myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY, BLACK);
			if (type == _rTime)
				drawParamValue(offsetTextY(1),  _u8, (void*) &temporary, list.l[num].pref, false);
			else
				drawParamValue(offsetTextY(1), type, (void*) &temporary, list.l[num].pref, false);
			break;

		/*case _123:
			if (k < '0' || k > '9')
				break;
			if (temporary > 10)
				break;
			temporaryArr[temporary++] = k;
			(*temporaryArr)++;

			myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY);
			drawParamValue(offsetTextY(1), type, (void*) &temporaryArr, list.l[num].pref, false);
			break;*/

		case _Aenum:
		case _Denum:
			if (key == Key::RIGTH) {
				temporary++;
				if (temporary >= list.flagAD->count[type])
					temporary = 0;
			} else if (key == Key::LEFT) {
				temporary--;
				if (temporary >= list.flagAD->count[type])
					temporary = list.flagAD->count[type] - 1;
			} else
				break;

			myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY, BLACK);
			drawParamValue(offsetTextY(1), type, (void*) &list.flagAD->_enum[type][temporary], list.l[num].pref, false);
			break;

		case _bool: {
			if (!((key == Key::RIGTH) || (key == Key::LEFT)))
				break;
			bool *b = (bool*) &temporary;
			*b = !(*b);
			myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY, BLACK);
			drawParamValue(offsetTextY(1), type, (void*) b, list.l[num].pref, false);
		}
			break;

		case _lang: {
			if (!((key == Key::RIGTH) || (key == Key::LEFT)))
				break;
			temporary = temporary ? 0 : 1;
			myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY, BLACK);
			drawParamValue(offsetTextY(1), type, (void*) &temporary,
					list.l[num].pref, false);
		}
			break;

		case _nstr:
		case _str:
			//key = (uint8_t)changeStr((uint8_t)key, temporaryArr);
			break;

		case _mask:
			//k = changeCheckbox(key, temporary);
			break;
		}

		switch (key) {
		case Key::OK:
			switch (type) {
			case _lang:
			case _u8:
				*((uint8_t*) list.l[num].data) = temporary;
				break;

			case _rTime:
				*((uint32_t*) list.l[num].data) = temporary * 60;
				break;

			case _u16:
				*((uint16_t*) list.l[num].data) = temporary;
				break;

			case _123:
				memcpy(list.l[num].data, temporaryArr, temporary);
				*(((char*) list.l[num].data) + temporary) = 0;
				break;

			case _Aenum:
			case _Denum:
				*((uint8_t*) list.l[num].data) =
						list.flagAD->_enum[type][temporary];
				break;

			case _bool:
				*((bool*) list.l[num].data) = *((bool*) &temporary);
				break;

			case _nstr:
				strcpy(((char*) list.l[num].data) + 1,
						const_cast<const char*>(temporaryArr));
				*((uint8_t*) list.l[num].data) = (uint8_t) strlen(
						const_cast<const char*>(temporaryArr));
				break;

			case _str:
				strcpy((char*) list.l[num].data,
						const_cast<const char*>(temporaryArr));
				break;

			case _mask:
				*((uint8_t*) list.l[num].data) = (uint8_t) temporary;
				break;
			}

			changeConfig = true; //Изменили настроки, значит нужно сохранить
		case Key::BACK:
			//if ((type == _nstr) || (type == _str) || type == _mask) {
				drawMenu(num, list);
			//} else {
				//myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY);
				//drawParamValue(offsetTextY(1), type, list.l[num].data, list.l[num].pref, false);
				//myDisp.drawInvertRectangle(0, offsetItemY(0), DISP_W, hightY * 2);
				//RGB::update();
			//}
			return;
		default: break;
		}
	}
}


void processSettings(const paramMenuGame& list){
	Key::eKEY key;
	uint8_t num = 0;
	drawMenu(num, list);
	while(1){
		osMessageQueueGet(myKeyHandle, &key, 0, HAL_MAX_DELAY);

		switch (key) {
		case Key::LEFT:
		case Key::RIGTH:Sound::urgentlySong(effectClick);   break;
		case Key::OK:	Sound::urgentlySong(effectClickOn); break;
		//case Key::BACK:	Sound::urgentlySong(effectClickEsc);break;
		default: break;
		}


		switch (key) {
			case Key::LEFT:
				if (num > 0) {
					num--;
					drawMenu(num, list);
				}
				break;

			case Key::RIGTH:
				if (num < (list.len - 1)) {
					num++;
					drawMenu(num, list);
				}
				break;

			case Key::OK:
				{
					uint8_t y = offsetTextY(0);
					const char* str = list.l[num].name[Lang];
					myDisp.drawRectangle(0, offsetItemY(0), DISP_W, hightY, WHITE);
					myDisp.drawRectangle(0, offsetItemY(1), DISP_W, hightY, BLACK);
					myDisp.drawText(0, y, str, Font_7x10, true, BLACK, WHITE);
					drawParamValue(offsetTextY(1), list.l[num].type, list.l[num].data, list.l[num].pref, false);
				}

				switch (list.l[num].type) {
				case _lang:
				case _bool:
				case _u8:
					temporary = *((uint8_t*) list.l[num].data);
					break;

				case _rTime:
					temporary = *((uint32_t*) list.l[num].data) / 60;
					break;

				case _u16:
					temporary = *((uint16_t*) list.l[num].data);
					break;
				case _123:
					*temporaryArr = 0;
					temporary = 1;
					break;

				case _Aenum:
				case _Denum:
					uint8_t i;
					for (i = 0; i < list.flagAD->count[list.l[num].type]; i++)
						if (list.flagAD->_enum[list.l[num].type][i] == *((uint8_t*) list.l[num].data))
							break;
					temporary = i;
					break;

				case _nstr:
					strcpy(temporaryArr, (const char*) list.l[num].data + 1);
					firstChangeStr(temporaryArr);
					break;

				case _str:
					strcpy(temporaryArr, (const char*) list.l[num].data);
					firstChangeStr(temporaryArr);
					break;

				case _mask:
					temporary = *((uint8_t*) list.l[num].data);
					firstCheckbox(temporary);
					break;
				}

				editParamValue(num, list);
				break;

			case Key::BACK: //обновиться при выходе из настроек
				if (changeConfig) {
					changeConfig = false;

					Disk::saveCfg();

					Transmitter::updPower(cfgSave.Cfg_IRpower);
					if (cfgSave.wifi_en) {
						if (phyDev.wifi_server_initOk == true)
							wifi::waitWifi();
					} else {
						wifi::power(false);
						phyDev.Real_Rssi = -128;
						phyDev.wifi_server_initOk = true;
					}
				}
				osMessageQueuePut(myKeyHandle, &key, 0, HAL_MAX_DELAY);
				return;
			default: break;
		}
	}
}




//CHANGE STRING*******************************************************************
const char* ENG = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //-32
const char* eng = "abcdefghijklmnopqrstuvwxyz"; //-32
const char* RUS = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"; //-32
const char* rus = "абвгдежзийклмнопрстуфхцчшщъыьэюя";
//const char* SYM = "0123456789!\"#$%&'()*+,-./";
//const char* sym = ":;<=>?@[\\]^_`{|}~";
const char* SYM = "0123456789&#$%,._-+()/~*\"':;!?";
const char* sym = "<=>@[\\]^`{|}";

constexpr uint8_t sX = 3;
constexpr uint8_t sY = 20;
constexpr uint8_t lX = 10;
constexpr uint8_t lY = 10;
constexpr uint8_t lenX = 12;
constexpr uint8_t lenY = 4;

constexpr uint8_t maxStr = 15;

void drawKeyboard(const char* changeStr, const char* lng, uint8_t posX, uint8_t posY){
	/*myDisp.drawClearRectangle(0, 20, DISP_W, DISP_H - 20);
	for (uint8_t y = 0; y < (lenY+1); y++) myDisp.drawLineH(sX, sX + lX * lenX, sY + lY * y);
	for (uint8_t x = 0; x < (lenX+1); x++) myDisp.drawLineV(sX + lX * x, sY, sY + (lenY-1) * lY);
	for (uint8_t x = 0; x < (lenX+1);x+=2) myDisp.drawLineV(sX + lX * x, sY + (lenY-1) * lY, sY + lenY * lY);

	uint8_t line = 0;
	for (uint8_t i = 0; i < strlen(lng); i++){
		if ((i != 0) && ((i % lenX) == 0)) line++;
		//printf("%d\t%d\t%d\t%d\r\n", lng[i], line, sX + lX * (i % lenX) + 2, sY + line * lY + 2 );
		myDisp.drawChar(sX + lX * (i % lenX) + 2, sY + line * lY + 2, lng[i], Font_5x7);
	}

	line = (lenY-1);
	myDisp.drawImage(sX + lX * 0 + 2, sY + line * lY + 2, &img_key_space);
	myDisp.drawImage(sX + lX * 2 + 2, sY + line * lY + 2, &img_key_del);
	myDisp.drawImage(sX + lX * 4 + 2, sY + line * lY + 2, &img_key_bl);
	myDisp.drawImage(sX + lX * 6 + 2, sY + line * lY + 2, &img_key_lng);
	myDisp.drawImage(sX + lX * 8 + 2, sY + line * lY + 2, &img_key_num);
	myDisp.drawImage(sX + lX *10 + 2, sY + line * lY + 2, &img_key_ok);


	if (posY == (lenY - 1)) {
		uint8_t _x = posX % 2 == 1 ? posX - 1 : posX;
		myDisp.drawInvertRectangle  (_x * lX + sX + 1, posY * lY + sY + 1, 2 * lX - 2, lY - 2);
	} else
		myDisp.drawInvertRectangle(posX * lX + sX + 1, posY * lY + sY + 1,     lX - 2, lY - 2);
	myDisp.flush();*/
}

void drawKeyStr(char* changeStr){
	/*myDisp.drawClearRectangle(0, 8, DISP_W, 10);
	myDisp.drawText(sX, 8, (const char*) changeStr, Font_7x10);
	myDisp.flush();*/
}

const char* getLang(uint8_t flag){
	switch (flag){
	case 0x00: return ENG;
	case 0x80: return eng;
	case 0x01: return RUS;
	case 0x81: return rus;
	case 0x02: return SYM;
	case 0x82: return sym;
	}
	return ENG;
}

static const char *lang = ENG;
static uint8_t posX, posY, flaglang;
static size_t index;
void firstChangeStr(char* changeStr){
	flaglang = 0;
	lang = ENG;

	myDisp.fillScreen(WHITE);
	myDisp.drawText(sX, 8, (const char*) changeStr, Font_7x10);

	posX = 0, posY = 0;
	drawKeyboard((const char*) changeStr, lang, posX, posY);
	index = strlen((const char*) changeStr);
}

uint8_t changeStr(char k, char* changeStr){
	/*uint8_t prevX = posX, prevY = posY;

	if (k == key_Exit){
		return key_Exit;
	}

	if (k == '5' || k == key_OK) {
		if (posY == (lenY - 1)) { // Управление
			switch (prevX / 2) {
			case sKey_space:
				if (index < maxStr) {
					changeStr[index++] = ' ';
					changeStr[index] = 0;
					drawKeyStr(changeStr);
				}
				break;
			case sKey_back:
				if (index > 0) {
					changeStr[--index] = 0;
					drawKeyStr(changeStr);
				}
				break;
			case sKey_shift:
				flaglang ^= 0x80;
				lang = getLang(flaglang);
				drawKeyboard((const char*) changeStr, lang, posX, posY);
				break;
			case sKey_lang:
				switch (flaglang) {
				case 0x00:
					flaglang = 0x01;
					break;
				case 0x80:
					flaglang = 0x81;
					break;
				case 0x02:
				case 0x01:
					flaglang = 0x00;
					break;
				case 0x82:
				case 0x81:
					flaglang = 0x80;
					break;
				}
				lang = getLang(flaglang);
				drawKeyboard((const char*) changeStr, lang, posX, posY);
				break;
			case sKey_num:
				switch (flaglang) {
				case 0x00:
				case 0x01:
					flaglang = 0x02;
					break;
				case 0x80:
				case 0x81:
					flaglang = 0x82;
					break;
				}
				lang = getLang(flaglang);
				drawKeyboard((const char*) changeStr, lang, posX, posY);
				break;
			case sKey_okay:
				return key_OK;
			}
		} else { //Символ
			if (index < maxStr) {
				uint8_t num = posX + lenX * posY;
				if (num < strlen(lang)) {
					changeStr[index++] = lang[num];
					changeStr[index] = 0;
					drawKeyStr(changeStr);
				}
			}
		}
	} else {
		if (posY == (lenY - 1)) {
			if ((k == '4') || (k == '7'))
				posX = posX > 0 ? posX - 1 : 0;
			if ((k == '6') || (k == '9'))
				posX = posX < (lenX - 1) ? posX + 1 : (lenX - 1);
		}
		if ((k == '1') || (k == '2') || (k == '3'))
			//posY = posY > 0 ? posY - 1 : 0;
			posY = posY > 0 ? posY - 1 : lenY - 1;
		if ((k == '1') || (k == '4') || (k == '7'))
			//posX = posX > 0 ? posX - 1 : 0;
			posX = posX > 0 ? posX - 1 : lenX - 1;
		if ((k == '3') || (k == '6') || (k == '9'))
			//posX = posX < (lenX - 1) ? posX + 1 : (lenX - 1);
			posX = posX < (lenX - 1) ? posX + 1 : 0;
		if ((k == '7') || (k == '8') || (k == '9'))
			//posY = posY < (lenY - 1) ? posY + 1 : (lenY - 1);
			posY = posY < (lenY - 1) ? posY + 1 : 0;

		if ((prevX != posX) || (prevY != posY)) {
			if (prevY == (lenY - 1)) {
				uint8_t _x = prevX % 2 == 1 ? prevX - 1 : prevX;
				myDisp.drawInvertRectangle(_x * lX + sX + 1, prevY * lY + sY + 1,
						2 * lX - 2, lY - 2);
			} else
				myDisp.drawInvertRectangle(prevX * lX + sX + 1, prevY * lY + sY + 1,
						lX - 2, lY - 2);

			if (posY == (lenY - 1)) {
				uint8_t _x = posX % 2 == 1 ? posX - 1 : posX;
				myDisp.drawInvertRectangle(_x * lX + sX + 1, posY * lY + sY + 1,
						2 * lX - 2, lY - 2);
			} else
				myDisp.drawInvertRectangle(posX * lX + sX + 1, posY * lY + sY + 1,
						lX - 2, lY - 2);
			myDisp.flush();
		}
	}*/
	return 0;
}



//CHANGE CheckBox*******************************************************************
static uint8_t cbNum;

void drawCheckBox(uint8_t num, uint16_t& mask){
	/*myDisp.fillScreen(WHITE);

	if (num)
		myDisp.drawImage(HALF_DISP_W - img_top.width / 2, 0, &img_top);
	if (num != (cbMax-1))
		myDisp.drawImage(HALF_DISP_W - img_bottom.width / 2, DISP_H - 8, &img_bottom);

	myDisp.drawImage(0, DISP_H - 8, &img_ok);
	myDisp.drawImage(DISP_W - 8, DISP_H - 8, &img_return);

	uint8_t max = cbMax - num;
	if (max > 4) max = 4;
	uint8_t i = 0;
	while (i < max) {
		myDisp.drawRectangle(4, offsetTextY(i), 9, 9);
		if (mask & (1 << (i + num)))
			myDisp.drawImage(4+1, offsetTextY(i)+1, &img_ok, false);
		else
			myDisp.drawClearRectangle(4+1, offsetTextY(i)+1, 7, 7);

		myDisp.drawText(4 + 12, offsetTextY(i), langStgMaskTeam[i + num][Lang], Font_7x10, false);
		i++;
	}
	myDisp.drawInvertRectangle(0, offsetItemY(0), DISP_W, 11);*/
}

void firstCheckbox(uint16_t& mask){
	cbNum = 0;
	drawCheckBox(cbNum, mask);
}

uint8_t changeCheckbox(char k, uint16_t& mask){
	/*switch (k) {
	case key_Up:
		if (cbNum > 0)
			cbNum--;
		drawCheckBox(cbNum, mask);
		break;

	case key_Down:
		if (cbNum < (cbMax - 1))
			cbNum++;
		drawCheckBox(cbNum, mask);
		break;

	case key_OK:
		mask ^= (1 << cbNum);
		drawCheckBox(cbNum, mask);
		break;

	case key_Exit:
		return key_OK;
	}
*/
	return 0;
}
