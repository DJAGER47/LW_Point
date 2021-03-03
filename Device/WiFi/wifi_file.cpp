/*
 * wifi_file.cpp
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: Admin
 */

#include "wifi.h"
#include "wifi_protocol.h"
#include "uarthandler.h"
#include "CRC32.h"
#include "version.h"
#include "game.h"

#include "../disk/Disk.h"

//фиксировано
static uint8_t  _fileId;
static uint32_t _fileSize;
static uint32_t _fileHandle;
static uint32_t _fileCRC;
//—читаем
static uint32_t filePos;
static uint32_t fileCRC;

#if 0
#define INFO_D(x) INFO(x)
#else
#define INFO_D(x)
#endif


void doCREATE_FILE(WIFI_DATA<0>* data) {
	INFO_D("CREATE_FILE ");
	if((phyDev.Real_State == STATE_RunGame)||(phyDev.Real_State == STATE_WaitStart)){
		SendWifi(CREATE_FILE, E_ACCESS_DENIED, sizeof(_CREATE_FILE), data->data,0,0);
		return;
	}

	_CREATE_FILE* createFile = (_CREATE_FILE*) data->data;
	_FILE_INFO*   info = (_FILE_INFO*) &createFile->Slot;

	if (createFile->Slot) {
		SendWifi(CREATE_FILE, E_UNK_UNIT, sizeof(_CREATE_FILE), data->data, 0, 0);
		INFO_D("E_UNK_UNIT\r\n");
		return;
	}

	if ((createFile->FileID < 0x10000) || (createFile->FileID > 0x100FF)) {
		SendWifi(CREATE_FILE, E_INVALID_ARG_3, sizeof(_CREATE_FILE), data->data, 0, 0);
		INFO_D("E_INVALID_ARG_3\r\n");
		return;
	}

	if (createFile->FileSize > Disk::getFreeSize()){
		SendWifi(CREATE_FILE, E_FILE_OP_ERROR, sizeof(_CREATE_FILE), data->data, 0, 0);
		INFO_D("E_FILE_OP_ERROR\r\n");
		return;
	}

	bool res = Disk::fileCreate(info->FileID, info->FileSize, info->FileCRC);
	if (!res) {
		SendWifi(CREATE_FILE, E_FILE_OP_ERROR, sizeof(_CREATE_FILE), data->data, 0, 0);
		INFO_D("error\r\n");
		return;
	}

	Disk::fileWrite(createFile->Data, data->len - sizeof(_CREATE_FILE));

	_fileId = createFile->FileID & 0xFF;
	_fileHandle = createFile->Handle;
	_fileSize = createFile->FileSize;
	_fileCRC = createFile->FileCRC;

	filePos = data->len - sizeof(_CREATE_FILE);
	fileCRC = crc_32(~0, createFile->Data, filePos);

	//printf("pos %d\r\n", (int)filePos);
	SendWifi(CREATE_FILE, S_OK, sizeof(createFile->Handle), &createFile->Handle, 0, 0);
}

void doWRITE_FILE(WIFI_DATA<0>* data) {
	INFO_D("WRITE_FILE ");
	_WRITE_FILE *writeFile = (_WRITE_FILE*) data->data;
	if ((phyDev.Real_State == STATE_RunGame) || (phyDev.Real_State == STATE_WaitStart)) {
		SendWifi(WRITE_FILE, E_ACCESS_DENIED, sizeof(writeFile->Handle), &writeFile->Handle, 0, 0);
		return;
	}

	if (writeFile->Handle != _fileHandle){
		SendWifi(WRITE_FILE, E_INVALID_ARG_3, sizeof(_WRITE_FILE), data->data, 0, 0);
		INFO_D("E_INVALID_ARG_3\r\n");
		return;
	}

	if (filePos != writeFile->FilePos_RecvPos){
		//отправить нужный адрес
		INFO_D("filePos != \r\n");
		SendWifi(WRITE_FILE, S_PENDING, sizeof(_WRITE_FILE), writeFile, 0, 0);
		return;
	}

	uint32_t len = data->len - sizeof(_WRITE_FILE);
	if (len == 0){
		INFO_D("len 0\r\n");
		SendWifi(WRITE_FILE, S_OK, sizeof(_WRITE_FILE), writeFile, 0, 0);
		return;
	}

	bool res = Disk::fileWrite(writeFile->Data, len); //writeFile->FilePos_RecvPos
	if (!res){
		//что делать? недописали
		SendWifi(WRITE_FILE, E_FILE_OP_ERROR, sizeof(_WRITE_FILE), writeFile, 0, 0);
		INFO_D("res\r\n");
		return;
	}

	filePos += len;
	fileCRC = crc_32(fileCRC, writeFile->Data, len);

	writeFile->FilePos_RecvPos = filePos;

	if(_fileSize <= filePos){
		if (_fileCRC == fileCRC){
			INFO_D("WRITE_FILE Save\r\n");
			bool res = Disk::fileClose();
			if (res) INFO_D("WRITE_FILE RENAME\r\n");
			else INFO_D("WRITE_FILE RENAME error\r\n");

			SendWifi(WRITE_FILE, S_OK, sizeof(_WRITE_FILE), writeFile, 0, 0);
		}else{
			INFO_D("WRITE_FILE Save error\r\n");
			SendWifi(WRITE_FILE, E_FILE_OP_ERROR, sizeof(_WRITE_FILE), writeFile, 0, 0);
		}
	}else{
		SendWifi(WRITE_FILE, S_PENDING, sizeof(_WRITE_FILE), writeFile, 0, 0);
	}
}

void doREAD_FILE_INFO(WIFI_DATA<0>* data) {
	INFO_D("READ_FILE_INFO");

	_FILE_INFO* info = (_FILE_INFO*)data->data;
	if (info->Slot){
		INFO_D("Error slot\r\n");
		SendWifi(READ_FILE_INFO, E_UNK_UNIT, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	if ((info->FileID < 0x10000) || (info->FileID > 0x100FF)) {
		INFO_D("Error id\r\n");
		SendWifi(READ_FILE_INFO, E_INVALID_ARG_3, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	uint8_t id = info->FileID & 0xFF;
	bool file = Disk::listFile(id);
	if (!file) {
		INFO_D("Not file\r\n");
		SendWifi(READ_FILE_INFO, E_FILE_NOT_FOUND, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	uint8_t res = Disk::fileInfo(info->FileID, info->FileSize, info->FileCRC);
	if (!res){
		INFO_D("Error read info\r\n");
		SendWifi(READ_FILE_INFO, E_FILE_NOT_FOUND, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	INFO_D("Ok\r\n");
	SendWifi(READ_FILE_INFO, S_OK, sizeof(_FILE_INFO), data->data, 0, 0);
}

void doEXECUTE_FILE(WIFI_DATA<0>* data) {
	INFO_D("EXECUTE_FILE\r\n");

	_FILE_REQ *info = (_FILE_REQ*) data->data;
	if (info->Slot != 0) {
		SendWifi(EXECUTE_FILE, E_UNK_UNIT, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	if ((info->FileID < 0x10000) || (info->FileID > 0x100FF)) {
		SendWifi(EXECUTE_FILE, E_INVALID_ARG_3, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	uint8_t id = info->FileID & 0xFF;
	Sound::addSong(id);
	SendWifi(EXECUTE_FILE, S_OK, sizeof(_FILE_REQ), data->data, 0, 0);
}

void doDELETE_FILE(WIFI_DATA<0>* data) {
	INFO_D("DELETE_FILE\r\n");
	//if((phyDev.Real_State==DEV_ST_RUN)||(phyDev.Real_State==DEV_ST_WAIT))
	//uart_send(AT_COMM_CMD_WRITE_FILE,E_ACCESS_DENIED,4,&RecvPack.FileData,0,0);

	_FILE_REQ* info = (_FILE_REQ*) data->data;
	if (info->Slot != 0){
		SendWifi(DELETE_FILE, E_UNK_UNIT, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	if ((info->FileID < 0x10000) || (info->FileID > 0x100FF)){
		SendWifi(DELETE_FILE, E_INVALID_ARG_3, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	uint8_t id = info->FileID & 0xFF;
	bool file = Disk::listFile(id);
	if (!file) {
		SendWifi(DELETE_FILE, E_FILE_NOT_FOUND, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	bool result = Disk::fileDel((uint8_t)info->FileID);
	if (!result){
		SendWifi(DELETE_FILE, E_FILE_NOT_FOUND, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	SendWifi(DELETE_FILE, S_OK, sizeof(_FILE_REQ), data->data, 0, 0);
}

void doGET_AUDIO_FILE_LIST(WIFI_DATA<0>* data) {
	INFO_D("GET_AUDIO_FILE_LIST\r\n");

	_FILE_REQ* info = (_FILE_REQ*) data->data;
	if (info->Slot != 0) {
		info->FileID = 0;
		SendWifi(GET_AUDIO_FILE_LIST, E_UNK_UNIT, sizeof(_FILE_REQ), data->data, 0, 0);
		return;
	}

	uint8_t *BufTx = uart_handler::getBufTx();
	WIFI_DATA<0> *dataSend = (WIFI_DATA<0>*) BufTx;

	uint16_t index = 0;
	for( auto i = 0; i < 256; i++){
		if (Disk::listFile(i))
			dataSend->data[index++] = i;
	}

	uint16_t size = index;
	uint16_t len = size + 5;
	dataSend->len = size;
	dataSend->cmd = GET_AUDIO_FILE_LIST;
	dataSend->status = S_OK;

	dataSend->data[size] = 0;
	for (uint16_t i = 0; i < (len - 1); i++)
		dataSend->data[size] -= BufTx[i];

	while (uart_handler::busyTX());
	uart_handler::startSend(len);
}
