/*
 * Sound.h
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef SOUND_SOUND_H_
#define SOUND_SOUND_H_

#include <stdint.h>
#include "Audio.h"

constexpr uint16_t lengBlock = 2048;
constexpr uint16_t lengBufSong = lengBlock   / 4;//512
constexpr uint16_t lengImaSong = lengBufSong / 2;//256 Динна буфера IMA

class Sound {
public:
	static void init(osMessageQueueId_t mySoundHandle);
	static void addSong(uint8_t id, uint16_t delay = 0);
	static void urgentlySong(uint8_t id);
	static bool isEmpty();
	static bool isRun();
	static void Stop();

	static void handle();
};


#endif /* SOUND_SOUND_H_ */
