/*
 * Disk.h
 *
 *  Created on: 15 февр. 2020 г.
 *      Author: Admin
 */

#ifndef DISK_DISK_H_
#define DISK_DISK_H_

#include <stdint.h>

constexpr uint16_t maxSector = 500;
typedef struct{
	uint16_t id;
	uint16_t reserved;
	uint32_t size;
	uint32_t crcFile;		  //+12  byte
	uint16_t addr[maxSector]; //1000 byte

	uint32_t crc;
}HeaderFile; //free 8 byte


constexpr uint8_t  startDataSector = 48;
constexpr uint16_t availableSector = 2000;
typedef struct{
	uint8_t mapSector[availableSector/8];
	uint32_t crc;
}MapSector;

/*	0,1 cfg
	2,3 map sector
	4 - write file
	5-7 Reserve
	8-47 Header file 1-160
	48+ sector data*/
class Disk {
	static bool existFile(uint8_t id);
public:
	static void init();

	static uint32_t getFreeSize();
	static bool fileInfo(uint8_t id, uint32_t &FileSize, uint32_t &FileCRC);
	static bool listFile(uint8_t id);
	static void saveCfg();
	static void saveMap();

	static bool fileOpen(uint8_t id, uint32_t &sizeFile);
	static bool fileRead(uint8_t* data, uint16_t len);

	static bool fileCreate(uint8_t id, uint32_t FileSize, uint32_t FileCRC);
	static bool fileWrite(uint8_t* data, uint16_t len);
	static bool fileClose();

	static bool fileDel(uint8_t id);
};

#endif /* DISK_DISK_H_ */

