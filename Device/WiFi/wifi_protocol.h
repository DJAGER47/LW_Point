/*
 * wifi_protocol.h
 *
 *  Created on: 18 февр. 2020 г.
 *      Author: lavrovskij
 */

#ifndef WIFI_WIFI_PROTOCOL_H_
#define WIFI_WIFI_PROTOCOL_H_

#include <stdint.h>
#include "define.h"

#pragma pack(push,1)


typedef enum { //WIFI_Cmd
	SYNC_TIME = 0,//Синхронизация времени
	SET_ENV = 1,//Установка информации о хосте
	SET_CONFIG = 2,//Установка конфигурации
	GET_FW_INFO = 3,//Получение информации о прошивке
	GET_FW_DATA = 4,//Получение данных прошивки
	CONNECT = 5,//Информация о соединении
	SET_SERVER_CTRL = 6,//Установка серверного управления
	START_GAME = 7,//Старт игры
	STOP_GAME = 8,//Останов игры
	UPDATE_ITEMS = 9,//	Обновление параметров
	CLEAR_STAT = 10,//Очистка истории событий
	UPDATE_EVENTS = 11,//Обновление событий
	WRITE_ITEMS = 12,//Запись параметров
	CREATE_FILE = 13,//Создание файла
	WRITE_FILE = 14,//Запись файла
	READ_FILE_INFO  = 15,//Получение информации о файле
	EXECUTE_FILE = 16,//Выполнение файла
	DELETE_FILE = 17,//Удаление файла
	GET_AUDIO_FILE_LIST = 18,//Получение списка аудиофайлов
	REBOOT = 19//Перезагрузка
}WIFI_Cmd;

typedef enum { //WIFI_Status
	S_OK = 0x00,//Нет ошибок. Удачное завершение
	S_FALSE = 0x01,//Есть ошибки. Удачное завершение
	S_PENDING = 0x02,//Команда выполняется
	S_REQUEST = 0x03,//Признак пакета с запросом

	E_INVALID_CMD = 0x80,//Команда не распознана
	E_NO_RESOURCES = 0x81,//Недостаточно ресурсов для выполнения
	E_ACCESS_DENIED = 0x82,//Нет доступа
	E_UNK_UNIT = 0x83,//Устройство отсутствует
	E_UNK_ITEM = 0x84,//Параметр отсутствует
	E_FILE_NOT_FOUND = 0x85,//Файл отсутствует
	E_UNIT_ERROR = 0x86,//Устройство не доступно
	E_FILE_OP_ERROR = 0x87,//Ошибка операции с файлом
	LW_E_ALREADY_EXIST = 0x88,//Файл уже существует
	LWP_E_CRC_FAIL = 0x89,//Ошибка CRC

	E_OUT_OF_RANGE = 0x90,//Параметр вне диапазона
	E_TIMEOUT = 0xD0,//Таймаут
	E_LANG_UNK = 0xD1,//Неизвестный язык
	E_INVALID_ARG_x = 0xE0,//	Неправильный аргумент х
	E_INVALID_ARG_1 = 0xE1,
	E_INVALID_ARG_2 = 0xE2,
	E_INVALID_ARG_3 = 0xE3,
	E_INVALID_ARG_4 = 0xE4,
	E_FAIL = 0xFF//Внутренняя ошибка
}WIFI_Status;


struct WIFI_FW_GLOBAL{
	uint32_t Size;//Размер бинарного образа
	uint32_t crc;//CRC32 бинарного образа
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


//-------------Протокол----------------------
typedef uint64_t TimeStamp;

struct _SYNC_TIME{
	TimeStamp RecvTime;//Время получения команды источником
	TimeStamp SendTime;//Время отсылки ответа источником
};

struct _SET_ENV{
	uint32_t DeviceID;//Идентификатор комплекса
	uint8_t SyncSource:1;//Источник времени для модуля
	uint8_t BLE_En:1;//Включение модуля BLE
};

struct _SET_CONFIG{
	uint8_t SSID[32];//Имя сети
	uint8_t Key[32];//Пароль
};

struct _GET_FW_INFO{
	uint32_t Version;//Версия прошивки
	uint32_t Size;//Размер бинарного образа
	uint32_t crc;//CRC32 бинарного образа
};

struct _GET_FW_DATA{
	uint32_t FilePos;//Позиция в файле образа
	uint8_t Data[1024];//Порция данных
};

struct _CONNECT{
	int8_t RSSI; //Значение -128 указывает на потерю соединения
};

struct _SET_SERVER_CTRL{
	uint8_t SrvCtrl;//Флаг серверного управления
};

struct _START_GAME{
	int32_t TimeLeft;//Время до старта игровой сессии в мсек
	uint32_t RoundTime;//Время раунда в секундах
};

typedef struct{
	uint32_t Handle;//Хендл операции
	uint8_t  data[0];//Массив значений
} _UPDATE_ITEMS, _WRITE_ITEMS;

struct STAT_REC{
	uint8_t EventID:4;//Идентификатор события (4бита)
	uint8_t EventParam0:4;//Параметр 0 события (4бита)
	uint8_t EventParam1;//Параметр 1 события
	uint16_t EventParam2;//Параметр 2 события
	uint32_t TimeStamp;//Метка времени (мсек от начала игры)
};

template<uint16_t size> struct _UPDATE_EVENTS{
	uint32_t Handle;//Хендл операции
	STAT_REC Recs[size];//Массив событий
};

struct _CREATE_FILE{
	uint32_t Handle;//Хендл операции
	uint32_t Slot;//Слот устройства
	uint32_t FileID;//Идентификатор файла
	uint32_t FileSize;//Размер файла
	uint32_t FileCRC;//CRC32 образа файла
	uint8_t  Data[0];//Первая порция данных
};

struct _WRITE_FILE{
	uint32_t Handle;//Хендл операции
	uint32_t FilePos_RecvPos;//Позиция указателя файла
	uint8_t Data[0];//Данные файла
};

struct _FILE_REQ{
	uint32_t Slot;//Слот устройства
	uint32_t FileID;//Идентификатор файла
};

struct _FILE_INFO{
	uint32_t Slot;//Слот устройства
	uint32_t FileID;//Идентификатор файла
	uint32_t FileSize;//Размер файла
	uint32_t FileCRC;//CRC32 образа файла
};

#pragma pack(pop)

//от хоста в модуль:
void doGET_FW_INFO(WIFI_DATA<0>* data);
void doGET_FW_DATA(WIFI_DATA<0>* data);
void sendGET_FW_DATA(uint32_t pos);

//bidirectional
void sendSYNC_TIME();
void doSYNC_TIME(WIFI_DATA<0>* data);

//от хоста в модуль:
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

//от модуля в хост:
void doCONNECT(WIFI_DATA<0>* data); //при подсоединении/отсоединении точки доступа WiFi, периодически при изменении RSSI
void doSET_SERVER_CTRL(WIFI_DATA<0>* data); //трасляция команды сервера
void doSTART_GAME(WIFI_DATA<0>* data); //при старте сетевой игры
void doSTOP_GAME(WIFI_DATA<0>* data); //при останове сетевой игры
void doWRITE_ITEMS(WIFI_DATA<0>* data); //при записи параметров
void doCREATE_FILE(WIFI_DATA<0>* data); //трасляция команды сервера
void doWRITE_FILE(WIFI_DATA<0>* data); //трасляция команды сервера
void doREAD_FILE_INFO(WIFI_DATA<0>* data); //трасляция команды сервера
void doEXECUTE_FILE(WIFI_DATA<0>* data); //трасляция команды сервера(исполнение аудиофайлов)
void doDELETE_FILE(WIFI_DATA<0>* data); //трасляция команды сервера
void doGET_AUDIO_FILE_LIST(WIFI_DATA<0>* data); //трасляция команды сервера

void SendWifi(WIFI_Cmd cmd, WIFI_Status status, uint8_t len1, void* data1, uint8_t len2, void* data2);


extern UPDATE_ITEM uItem;

#endif /* WIFI_WIFI_PROTOCOL_H_ */
