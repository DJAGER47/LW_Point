/*
 * Sound.cpp
 *
 *  Created on: 10 февр. 2020 г.
 *      Author: lavrovskij
 */

#include "define.h"
#include "Sound.h"
#include "adpcm.h"
#include "version.h"
#include "../disk/Disk.h"
#include "main.h"
#include "queue2.h"
#include "Audio.h"

#include "cmsis_os.h"

#define i2s hi2s2
extern I2S_HandleTypeDef i2s;


#define MUTE_PIN  MUTE_Pin
#define MUTE_PORT MUTE_GPIO_Port

#define SOUND_OFF HAL_GPIO_WritePin(MUTE_PORT, MUTE_PIN, GPIO_PIN_RESET)
#define SOUND_ON  HAL_GPIO_WritePin(MUTE_PORT, MUTE_PIN, GPIO_PIN_SET)
//==============================================================================
//==============================================================================


static bool songData(false);
static uint8_t numBuf(1);		//номер буфера PCM для заполнения
static uint32_t sizeFile;		// Длинна всего отавшегося файла
static uint32_t indexFile;		// Байт по порядку файла
static int16_t song[2][lengBufSong]; //Буфер PCM
static uint8_t transferBuf[lengImaSong];

static float Volume;
static uint8_t silenceMs(0);
constexpr uint8_t updateBufms = (uint8_t)(10.0 * lengBufSong /882);

static bool needUpdate(false);
static osMessageQueueId_t fifoSong;


void updateBuffer(void){
	if (!songData){
		SOUND_OFF;
		if (!osMessageQueueGetCount(fifoSong)) return;
		SONG s;
		osMessageQueueGet(fifoSong, &s, 0, 0);


		bool res = Disk::fileOpen(s.id, sizeFile);
		if (!res) return;

		silenceMs = s.delay / updateBufms;
		Volume = cfgSave.Cfg_Volume / 100.0;
		songData = true;
		indexFile = 0;
	}

	if (silenceMs){ //задержка воспроизведения
		silenceMs--;
		return;
	}
	SOUND_ON;


	for (uint16_t i = 0; i < lengBufSong; i += 4) { //Обновить буфер

		if (indexFile % lengImaSong == 0){ //кончились данные ima
			if ((sizeFile - indexFile)> lengImaSong) {
				Disk::fileRead(transferBuf, lengImaSong);
			} else {
				Disk::fileRead(transferBuf, sizeFile - indexFile);
			}

			if (indexFile % lengBlock == 0){ //Начался новый блок
				indexFile += 4;
				int32_t prev = *((int16_t*)transferBuf);
				int16_t index = transferBuf[2];
				ADPCM_init(index, prev);
			}
		}

		uint8_t ima = transferBuf[indexFile % lengImaSong];
		indexFile++;

		int16_t adpcm;
		adpcm = ADPCM_Decode(ima & 0x0F)* Volume;
		song[numBuf][i] = adpcm;
		song[numBuf][i + 1] = adpcm;

		adpcm = ADPCM_Decode((ima >> 4) & 0x0F)* Volume;
		song[numBuf][i + 2] = adpcm;
		song[numBuf][i + 3] = adpcm;


		if (indexFile >= sizeFile) { //don't play
			songData = false;
			return;
		}
	}
}


//Tx Transfer Half completed callbacks
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
//void I2S_TxHalfCpltCallback(struct __DMA_HandleTypeDef * hdma) {
	numBuf = 0;
	needUpdate = true;
}

//Tx Transfer completed callbacks
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
//void I2S_TxCpltCallback(struct __DMA_HandleTypeDef * hdma) {
	numBuf = 1;
	needUpdate = true;
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	printf("ERROR TRANSMIT SOUND DMA");
}



void Sound::init(osMessageQueueId_t mySoundHandle){
	SOUND_OFF;
	fifoSong = mySoundHandle;
	HAL_I2S_Transmit_DMA(&i2s, (uint16_t*)song, lengBufSong * 2);
	songData = false;
	printf("Start Sound\r\n");
}

void Sound::addSong(uint8_t id, uint16_t delay){
	SONG s = {.id = id, .delay = delay};
	if (osMessageQueueGetSpace(fifoSong))
		osMessageQueuePut(fifoSong, &s, 0, 0);
}

void Sound::urgentlySong(uint8_t id){
	if (songData){
		osMessageQueueReset(fifoSong);
		songData = false;
	}
	addSong(id);
}

bool Sound::isEmpty(){
	return osMessageQueueGetCount(fifoSong);
}

bool Sound::isRun(){
	return (osMessageQueueGetCount(fifoSong)) || songData;
}

void Sound::Stop(){
	songData = false;
	osMessageQueueReset(fifoSong);
	SOUND_OFF;
}

void Sound::handle(){
	if (needUpdate){
		updateBuffer();
		needUpdate = false;
	}
}
