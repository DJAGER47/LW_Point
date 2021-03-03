
/*
 * bmp.h
 *
 *  Created on: 2 ���. 2020 �.
 *      Author: Admin
 */

#ifndef BMP_H_
#define BMP_H_

#include <stdint.h>

#define img_data_block_size 8

typedef struct {
	const uint8_t *data;
	uint8_t width;
	uint8_t height;
} tImage;

typedef struct
{
    long int code;
    const tImage *image;
} tChar;

typedef struct
{
    int length;
    const tChar *chars;
} tFont;


extern const tFont Font_5x7;
extern const tFont Font_7x10;
extern const tFont Font_16x18;

//GAME
extern const tImage fimg_startup;


extern const tImage img_rad;
extern const tImage img_usb;
extern const tImage img_pass;
extern const tImage img_shot;


extern const tImage fimg_game1Win;
extern const tImage fimg_game1Losing;

extern const tImage fimg_game2Win;
extern const tImage fimg_game2Losing;

extern const tImage fimg_game3Win;
extern const tImage fimg_game3Losing;


//INFO
extern const tImage img_rssiOff;
extern const tImage img_rssi0;
extern const tImage img_rssi1;
extern const tImage img_rssi2;
extern const tImage img_rssi3;
extern const tImage img_rssi4;
extern const tImage img_rssi5;

extern const tImage img_bat0;
extern const tImage img_bat1;
extern const tImage img_bat2;
extern const tImage img_bat3;
extern const tImage img_bat4;
extern const tImage img_bat5;
extern const tImage img_bat6;


//Settings
extern const tImage img_s1i1;
extern const tImage img_s1i2;
extern const tImage img_s1i3;
extern const tImage img_s1i4;

extern const tImage img_stg;
extern const tImage img_play;

extern const tImage img_left;
extern const tImage img_right;
extern const tImage img_top;
extern const tImage img_bottom;

extern const tImage img_ok;
extern const tImage img_exit;
extern const tImage img_return;

extern const tImage img_System;
extern const tImage img_wifi;
extern const tImage img_info;
extern const tImage img_MenuInfo;


extern const tImage img_key_ok;
extern const tImage img_key_del;
extern const tImage img_key_lng;
extern const tImage img_key_bl;
extern const tImage img_key_num;
extern const tImage img_key_space;



#endif /* BMP_H_ */
