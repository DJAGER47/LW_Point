/*
 * wifi_protocol.h
 *
 *  Created on: 18 ����. 2020 �.
 *      Author: lavrovskij
 */

#ifndef WIFI_WIFI_PROTOCOL_H_
#define WIFI_WIFI_PROTOCOL_H_

#include <stdint.h>
#include "define.h"

#pragma pack(push,1)


typedef enum { //WIFI_Cmd
	SYNC_TIME = 0,//������������� �������
	SET_ENV = 1,//��������� ���������� � �����
	SET_CONFIG = 2,//��������� ������������
	GET_FW_INFO = 3,//��������� ���������� � ��������
	GET_FW_DATA = 4,//��������� ������ ��������
	CONNECT = 5,//���������� � ����������
	SET_SERVER_CTRL = 6,//��������� ���������� ����������
	START_GAME = 7,//����� ����
	STOP_GAME = 8,//������� ����
	UPDATE_ITEMS = 9,//	���������� ����������
	CLEAR_STAT = 10,//������� ������� �������
	UPDATE_EVENTS = 11,//���������� �������
	WRITE_ITEMS = 12,//������ ����������
	CREATE_FILE = 13,//�������� �����
	WRITE_FILE = 14,//������ �����
	READ_FILE_INFO  = 15,//��������� ���������� � �����
	EXECUTE_FILE = 16,//���������� �����
	DELETE_FILE = 17,//�������� �����
	GET_AUDIO_FILE_LIST = 18,//��������� ������ �����������
	REBOOT = 19//������������
}WIFI_Cmd;

typedef enum { //WIFI_Status
	S_OK = 0x00,//��� ������. ������� ����������
	S_FALSE = 0x01,//���� ������. ������� ����������
	S_PENDING = 0x02,//������� �����������
	S_REQUEST = 0x03,//������� ������ � ��������

	E_INVALID_CMD = 0x80,//������� �� ����������
	E_NO_RESOURCES = 0x81,//������������ �������� ��� ����������
	E_ACCESS_DENIED = 0x82,//��� �������
	E_UNK_UNIT = 0x83,//���������� �����������
	E_UNK_ITEM = 0x84,//�������� �����������
	E_FILE_NOT_FOUND = 0x85,//���� �����������
	E_UNIT_ERROR = 0x86,//���������� �� ��������
	E_FILE_OP_ERROR = 0x87,//������ �������� � ������
	LW_E_ALREADY_EXIST = 0x88,//���� ��� ����������
	LWP_E_CRC_FAIL = 0x89,//������ CRC

	E_OUT_OF_RANGE = 0x90,//�������� ��� ���������
	E_TIMEOUT = 0xD0,//�������
	E_LANG_UNK = 0xD1,//����������� ����
	E_INVALID_ARG_x = 0xE0,//	������������ �������� �
	E_INVALID_ARG_1 = 0xE1,
	E_INVALID_ARG_2 = 0xE2,
	E_INVALID_ARG_3 = 0xE3,
	E_INVALID_ARG_4 = 0xE4,
	E_FAIL = 0xFF//���������� ������
}WIFI_Status;


struct WIFI_FW_GLOBAL{
	uint32_t Size;//������ ��������� ������
	uint32_t crc;//CRC32 ��������� ������
};

template<uint16_t size> struct WIFI_DATA{
	uint16_t len;
	WIFI_Cmd cmd;
	WIFI_Status status;
	uint8_t data[size];
	uint8_t bcc;
};

typedef struct{
	bool recieved;
	WIFI_DATA<0>* data;
	uint32_t handle;
} STATUS_RECEIV;

typedef struct{
	uint32_t time;
	bool needUpdateItem;
}UPDATE_ITEM;


//-------------��������----------------------
typedef uint64_t TimeStamp;

struct _SYNC_TIME{
	TimeStamp RecvTime;//����� ��������� ������� ����������
	TimeStamp SendTime;//����� ������� ������ ����������
};

struct _SET_ENV{
	uint32_t DeviceID;//������������� ���������
	uint8_t SyncSource:1;//�������� ������� ��� ������
	uint8_t BLE_En:1;//��������� ������ BLE
};

struct _SET_CONFIG{
	uint8_t SSID[32];//��� ����
	uint8_t Key[32];//������
};

struct _GET_FW_INFO{
	uint32_t Version;//������ ��������
	uint32_t Size;//������ ��������� ������
	uint32_t crc;//CRC32 ��������� ������
};

struct _GET_FW_DATA{
	uint32_t FilePos;//������� � ����� ������
	uint8_t Data[1024];//������ ������
};

struct _CONNECT{
	int8_t RSSI; //�������� -128 ��������� �� ������ ����������
};

struct _SET_SERVER_CTRL{
	uint8_t SrvCtrl;//���� ���������� ����������
};

struct _START_GAME{
	int32_t TimeLeft;//����� �� ������ ������� ������ � ����
	uint32_t RoundTime;//����� ������ � ��������
};

typedef struct{
	uint32_t Handle;//����� ��������
	uint8_t  data[0];//������ ��������
} _UPDATE_ITEMS, _WRITE_ITEMS;

struct STAT_REC{
	uint8_t EventID:4;//������������� ������� (4����)
	uint8_t EventParam0:4;//�������� 0 ������� (4����)
	uint8_t EventParam1;//�������� 1 �������
	uint16_t EventParam2;//�������� 2 �������
	uint32_t TimeStamp;//����� ������� (���� �� ������ ����)
};

template<uint16_t size> struct _UPDATE_EVENTS{
	uint32_t Handle;//����� ��������
	STAT_REC Recs[size];//������ �������
};

struct _CREATE_FILE{
	uint32_t Handle;//����� ��������
	uint32_t Slot;//���� ����������
	uint32_t FileID;//������������� �����
	uint32_t FileSize;//������ �����
	uint32_t FileCRC;//CRC32 ������ �����
	uint8_t  Data[0];//������ ������ ������
};

struct _WRITE_FILE{
	uint32_t Handle;//����� ��������
	uint32_t FilePos_RecvPos;//������� ��������� �����
	uint8_t Data[0];//������ �����
};

struct _FILE_REQ{
	uint32_t Slot;//���� ����������
	uint32_t FileID;//������������� �����
};

struct _FILE_INFO{
	uint32_t Slot;//���� ����������
	uint32_t FileID;//������������� �����
	uint32_t FileSize;//������ �����
	uint32_t FileCRC;//CRC32 ������ �����
};

#pragma pack(pop)

//�� ����� � ������:
void doGET_FW_INFO(WIFI_DATA<0>* data);
void doGET_FW_DATA(WIFI_DATA<0>* data);
void sendGET_FW_DATA(uint32_t pos);

//bidirectional
void sendSYNC_TIME();
void doSYNC_TIME(WIFI_DATA<0>* data);

//�� ����� � ������:
void sendSET_ENV();
void doSET_ENV(WIFI_DATA<0>* data);
void sendSET_CONFIG();
void doSET_CONFIG(WIFI_DATA<0>* data);
void sendUPDATE_ITEMS_ALL();
void doUPDATE_ITEMS(WIFI_DATA<0>* data);
void sendCLEAR_STAT();
void doCLEAR_STAT(WIFI_DATA<0>* data);
void sendUPDATE_EVENTS();
void doUPDATE_EVENTS(WIFI_DATA<0>* data);

//�� ������ � ����:
void doCONNECT(WIFI_DATA<0>* data); //��� �������������/������������ ����� ������� WiFi, ������������ ��� ��������� RSSI
void doSET_SERVER_CTRL(WIFI_DATA<0>* data); //��������� ������� �������
void doSTART_GAME(WIFI_DATA<0>* data); //��� ������ ������� ����
void doSTOP_GAME(WIFI_DATA<0>* data); //��� �������� ������� ����
void doWRITE_ITEMS(WIFI_DATA<0>* data); //��� ������ ����������
void doCREATE_FILE(WIFI_DATA<0>* data); //��������� ������� �������
void doWRITE_FILE(WIFI_DATA<0>* data); //��������� ������� �������
void doREAD_FILE_INFO(WIFI_DATA<0>* data); //��������� ������� �������
void doEXECUTE_FILE(WIFI_DATA<0>* data); //��������� ������� �������(���������� �����������)
void doDELETE_FILE(WIFI_DATA<0>* data); //��������� ������� �������
void doGET_AUDIO_FILE_LIST(WIFI_DATA<0>* data); //��������� ������� �������

void SendWifi(WIFI_Cmd cmd, WIFI_Status status, uint8_t len1, void* data1, uint8_t len2, void* data2);


extern UPDATE_ITEM uItem;

#endif /* WIFI_WIFI_PROTOCOL_H_ */
