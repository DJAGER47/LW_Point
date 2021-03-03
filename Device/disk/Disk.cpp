/*
 * Disk.cpp
 *
 *  Created on: 15 февр. 2020 г.
 *      Author: Admin
 */
#include "define.h"
#include "Disk.h"
#include "CRC32.h"
#include "version.h"
#include "biteArray.h"

#include "sst26vf/sst26vf_driver.h"

static HeaderFile hFileCreate;
static HeaderFile hFilePlay;
static MapSector  map;

static uint8_t dataTmp[SST26VF_SECTOR_SIZE]; //Временный буфер для работы с фалами
static uint8_t existFiles[20];
static uint32_t ReadAddress;
static uint32_t WriteAddress;



void Disk::init(){
	SST26VF::init();

	{//read cfg
		bool readCfg = true;
		uint32_t size = sizeof(ConfigDevSave);
		for (uint8_t i = 0; i < 2; i++) {
			SST26VF::read_buffer(i * SST26VF_SECTOR_SIZE, &cfgSave, size);
			uint32_t crc = crc_32(~0, (uint8_t*) &cfgSave, size - 4);
			if (cfgSave.crc == crc) {
				readCfg = false;
				break;
			}
		}
		if (readCfg) { //default config
			printf("CONFIG DEFAULT\r\n");
			memcpy(&cfgSave, &cfgSaveDefault, sizeof(ConfigDevSave));
		}
	}

	{//read map
		bool readCfg = true;
		uint32_t size = sizeof(MapSector);
		for (uint8_t i = 2; i < 4; i++) {
			SST26VF::read_buffer(i * SST26VF_SECTOR_SIZE, &map, size);
			uint32_t crc = crc_32(~0, map.mapSector, size - 4);
			if (map.crc == crc) {
				readCfg = false;
				break;
			}
		}
		if (readCfg) { //default config
			printf("MAP DEFAULT\r\n");
			memset(&map, ~0, sizeof(MapSector));
		}
	}

	getFreeSize();
	printf("Start Disk\r\n");

	printf("Found file ");
	uint32_t size, crc;
	for (auto i = 0; i < 160; i++){
		uint8_t b = fileInfo(i, size, crc);
		if (b){
			SetBit(existFiles, i, true);
			printf("%d  ", (int) i);
		}else
			SetBit(existFiles, i, false);
	}
	printf("\r\n");
}

uint32_t getAddressFile(uint8_t id){
	uint8_t offset = 8;
	return (offset * SST26VF_SECTOR_SIZE + id * 1024);
}

uint16_t findFreeSize(uint16_t prevSector){
	for (auto i = prevSector + 1; i < (startDataSector + availableSector); i++)
		if (GetBit(map.mapSector, i - startDataSector))
			return i;
	return 0;
}

uint32_t Disk::getFreeSize(){
	uint32_t freeSize = 0;
	for (auto i = 0; i < availableSector; i++) {
		if (GetBit(map.mapSector, i))
			freeSize++;
	}
	printf("%d KB free\r\n", (int) freeSize * 4);

	freeSize *= 4096;
	return freeSize;
}

bool Disk::fileInfo(uint8_t id, uint32_t &FileSize, uint32_t &FileCRC){
	HeaderFile* file = (HeaderFile*)dataTmp;
	uint32_t size = sizeof(HeaderFile);
	uint32_t addr = getAddressFile(id);
	SST26VF::read_buffer(addr, file, size);

	uint32_t crc = crc_32(~0, dataTmp, size - 4);
	if (crc != file->crc)
		return false;

	FileSize = file->size;
	FileCRC = file->crcFile;
	return true;
}

bool Disk::listFile(uint8_t id){
	uint8_t b = GetBit(existFiles, id);
	//if (b) return true;
	//return false;
	return b ? true : false;
}

void Disk::saveCfg(){
	uint32_t size = sizeof(ConfigDevSave);
	cfgSave.crc = crc_32(~0, (uint8_t*)&cfgSave, size - 4);

	SST26VF::erase_sector(0);
	SST26VF::erase_sector(1);

	SST26VF::write_buffer(0 * SST26VF_SECTOR_SIZE, &cfgSave, size);
	SST26VF::write_buffer(1 * SST26VF_SECTOR_SIZE, &cfgSave, size);
}

void Disk::saveMap(){
	uint32_t size = sizeof(MapSector);
	map.crc = crc_32(~0, map.mapSector, size - 4);

	SST26VF::erase_sector(2);
	SST26VF::erase_sector(3);

	SST26VF::write_buffer(2 * SST26VF_SECTOR_SIZE, &map, size);
	SST26VF::write_buffer(3 * SST26VF_SECTOR_SIZE, &map, size);
}


bool Disk::fileOpen(uint8_t id, uint32_t &sizeFile){
	uint32_t size = sizeof(HeaderFile);
	uint32_t addr = getAddressFile(id);
	SST26VF::read_buffer(addr, &hFilePlay, size);

	uint32_t crc = crc_32(~0, (uint8_t*)&hFilePlay, size - 4);
	if (crc != hFilePlay.crc)
		return false;
	sizeFile = hFilePlay.size;
	ReadAddress = 0;
	return true;
}

bool Disk::fileRead(uint8_t* data, uint16_t len){
	uint8_t* buf = data;
	uint16_t Nsector  = ReadAddress / SST26VF_SECTOR_SIZE;
	uint16_t Naddress = ReadAddress % SST26VF_SECTOR_SIZE;
	uint32_t address = SST26VF_SECTOR_SIZE * hFilePlay.addr[Nsector] + Naddress;

	if ((SST26VF_SECTOR_SIZE - Naddress) >= len){
		SST26VF::read_buffer(address, buf, len);
		ReadAddress += len;
		return true;
	}

	/*uint32_t byteW = SST26VF_SECTOR_SIZE - Naddress;
	SST26VF::read_buffer(address, buf, byteW);

	ReadAddress += byteW;
	Nsector  = ReadAddress / SST26VF_SECTOR_SIZE;
	Naddress = ReadAddress % SST26VF_SECTOR_SIZE;
	address = SST26VF_SECTOR_SIZE * hFilePlay.addr[Nsector] + Naddress;
	buf += byteW;
	len -= byteW;

	while (len >= SST26VF_SECTOR_SIZE) {
		SST26VF::read_buffer(address, buf, SST26VF_SECTOR_SIZE);
		ReadAddress += SST26VF_SECTOR_SIZE;
		Nsector = ReadAddress / SST26VF_SECTOR_SIZE;
		Naddress = ReadAddress % SST26VF_SECTOR_SIZE;
		address = SST26VF_SECTOR_SIZE * hFilePlay.addr[Nsector] + Naddress;
		buf += SST26VF_SECTOR_SIZE;
		len -= SST26VF_SECTOR_SIZE;
	}

	if (len > 0) {
		SST26VF::read_buffer(address, buf, len);
		ReadAddress += len;
	}*/
	return true;
}
//end play

bool Disk::fileCreate(uint8_t id, uint32_t FileSize, uint32_t FileCRC){ //+
	hFileCreate.id = id;
	hFileCreate.size = FileSize;
	hFileCreate.crcFile = FileCRC;
	memset(hFileCreate.addr, 0, sizeof(hFileCreate.addr));
	//hFileCreate.crc = 0;

	WriteAddress = 0;
	hFileCreate.addr[0] = findFreeSize(startDataSector - 1);
	SST26VF::erase_sector(hFileCreate.addr[0]);
	return true;
}

bool Disk::fileWrite(uint8_t* data, uint16_t len){
	uint16_t Nsector, Naddress;
	if (((WriteAddress / SST26VF_SECTOR_SIZE) != 0) && (
		 (WriteAddress % SST26VF_SECTOR_SIZE) == 0)) {
		Nsector = WriteAddress / SST26VF_SECTOR_SIZE;
		hFileCreate.addr[Nsector] = findFreeSize(hFileCreate.addr[Nsector - 1]);
		SST26VF::erase_sector(hFileCreate.addr[Nsector]);
	}

	uint8_t *buf = data;
	Nsector = WriteAddress / SST26VF_SECTOR_SIZE;
	Naddress = WriteAddress % SST26VF_SECTOR_SIZE;
	uint32_t address = SST26VF_SECTOR_SIZE * hFileCreate.addr[Nsector] + Naddress;

	if ((SST26VF_SECTOR_SIZE - Naddress) >= len){
		SST26VF::write_buffer(address, buf, len);
		WriteAddress += len;
	}else{
		printf("Error > 4096\r\n");
		//__ASM volatile("BKPT #01");
		/*uint32_t byteW = SST26VF_SECTOR_SIZE - Naddress;
		SST26VF::write_buffer(address, buf, byteW);
		WriteAddress += byteW;
		Nsector  = WriteAddress / SST26VF_SECTOR_SIZE;
		Naddress = WriteAddress % SST26VF_SECTOR_SIZE;

		hFileCreate.addr[Nsector] = findFreeSize(hFileCreate.addr[Nsector - 1]);
		SST26VF::erase_sector(hFileCreate.addr[Nsector]);
		address = SST26VF_SECTOR_SIZE * hFileCreate.addr[Nsector] + Naddress;
		buf += byteW;
		len -= byteW;

		while (len >= SST26VF_SECTOR_SIZE) {
			SST26VF::write_buffer(address, buf, SST26VF_SECTOR_SIZE);
			WriteAddress += SST26VF_SECTOR_SIZE;
			Nsector = WriteAddress / SST26VF_SECTOR_SIZE;
			Naddress = WriteAddress % SST26VF_SECTOR_SIZE;
			hFileCreate.addr[Nsector] = findFreeSize(hFileCreate.addr[Nsector - 1]);
			SST26VF::erase_sector(hFileCreate.addr[Nsector]);
			address = SST26VF_SECTOR_SIZE * hFileCreate.addr[Nsector] + Naddress;
			buf += SST26VF_SECTOR_SIZE;
			len -= SST26VF_SECTOR_SIZE;
		}

		if (len > 0) {
			SST26VF::write_buffer(address, buf, len);
			WriteAddress += len;
		}*/
	}
	return true;
}

bool Disk::fileClose(){
	HeaderFile* file = (HeaderFile*)dataTmp;
	uint32_t size = sizeof(HeaderFile);
	hFileCreate.crc = crc_32(~0, (uint8_t*)&hFileCreate, size - 4);
	uint32_t addressHead = getAddressFile(hFileCreate.id);

	// MAP new file
	for (auto i = 0; i < maxSector; i++){
		if (hFileCreate.addr[i] == 0) break;
		SetBit(map.mapSector, hFileCreate.addr[i] - startDataSector, false);
	}
	// MAP old file
	SST26VF::read_buffer(addressHead, file, size);
	uint32_t crc = crc_32(~0, dataTmp, size - 4);
	if (crc == file->crc){
		for (auto i = 0; i < maxSector; i++) {
			if (file->addr[i] == 0) break;
			SetBit(map.mapSector, file->addr[i] - startDataSector, true);
		}
	}
	saveMap();
	//END MAP

	uint32_t Nsector  = addressHead / SST26VF_SECTOR_SIZE;
	uint32_t Naddress = addressHead % SST26VF_SECTOR_SIZE;
	uint32_t addressSector = addressHead - Naddress;

	SST26VF::read_buffer(addressSector, dataTmp, SST26VF_SECTOR_SIZE);
	memcpy(dataTmp + Naddress, &hFileCreate, size);
	SST26VF::erase_sector(Nsector);
	SST26VF::write_buffer(addressSector, dataTmp, SST26VF_SECTOR_SIZE);


	SetBit(existFiles, hFileCreate.id, true);
	return true;
}

bool Disk::fileDel(uint8_t id){
	HeaderFile* file = (HeaderFile*)dataTmp;
	uint32_t size = sizeof(HeaderFile);
	uint32_t addressHead = getAddressFile(id);
	SST26VF::read_buffer(addressHead, file, size);

	uint32_t crc = crc_32(~0, dataTmp, size - 4);
	if (crc != file->crc)
		return true;

	for (auto i = 0; i < maxSector; i++) {
		if (file->addr[i] == 0) break;
		SetBit(map.mapSector, file->addr[i] - startDataSector, true);
	}
	saveMap();

	uint32_t Nsector = addressHead / SST26VF_SECTOR_SIZE;
	uint32_t Naddress = addressHead % SST26VF_SECTOR_SIZE;
	uint32_t addressSector = addressHead - Naddress;

	SST26VF::read_buffer(addressSector, dataTmp, SST26VF_SECTOR_SIZE);
	memset(dataTmp + Naddress, 0xFF, size);
	SST26VF::erase_sector(Nsector);
	SST26VF::write_buffer(addressSector, dataTmp, SST26VF_SECTOR_SIZE);

	SetBit(existFiles, id, false);
	return true;
}

