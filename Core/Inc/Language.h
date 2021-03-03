/*
 * Language.h
 *
 *  Created on: 4 апр. 2020 г.
 *      Author: Admin
 */

#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#define s_USB    0x1B
#define s_PASS   0x1C
#define s_SHOT   0x1D

#define s_ppp   0x1E
#define s_angle 0x1F


extern const char* s_per[2];
extern const char* s_sec[2];
extern const char* s_min[2];
extern const char* s_true[2];
extern const char* s_false[2];
extern const char* s_lang[2];
extern const char* s_win[2];
extern const char* s_FW[2];

extern const char* s_Exit[2];
extern const char* s_Yes[2];
extern const char* s_No[2];



extern const char ADFlags[];
extern const char* langStgMenu[][2];
extern const char* langStgGame[5][2];
extern const char* langStgSystem[11][2];
extern const char* langStgWifi[3][2];
extern const char* langStgMaskTeam[][2];

extern const char* langGameStart[];
extern const char* langGameBomb[];
extern const char* langGameMOVE1[];
extern const char* langGameMOVE2[];
extern const char* langGameMOVE3[];

#endif /* LANGUAGE_H_ */
