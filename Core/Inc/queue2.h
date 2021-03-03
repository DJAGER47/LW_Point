/*
 * queue.h
 *
 *  Created on: 28 нояб. 2019 г.
 *      Author: lavrovskij
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

template<class T, uint8_t size>
class Queue2 {
private:
	T pBuf[size];

	uint8_t ukEnd;
	uint8_t ukBegin;
	uint8_t len;

public:
	Queue2() {
		reinit();
	}

	void reinit() {
		ukBegin = 0;
		ukEnd = 0;
		len = 0;
	}

	T& operator[](int index) { // Не тестировал. Может не работать
		if (index > (len-1) ) index = len;
		uint8_t i = (ukBegin + index) % size;
		return pBuf[i];
	}

	void put(T byte){
		if (isFull()) return;
		pBuf[ukEnd] = byte;
		ukEnd = (ukEnd + 1) % size;
		len++;
	}

	T get(){//Прочитать голову
		return pBuf[ukBegin];
	}

	T del(){//Прочитать и достать голову
		T data = pBuf[ukBegin];
		ukBegin = (ukBegin + 1) % size;
		len--;
		return data;
	}

	bool isFull(){
		if (len == size)
			return true;
		return false;
	}

	bool isEmpty(){
		if (len == 0)
			return true;
		return false;
	}

	uint8_t emptyLength(){
		return (size - len);
	}

	//Вся длинна
	uint8_t getSize(){
		return size;
	}

	uint8_t getLength(){
		return len;
	}

	uint8_t getMemSize(){
		return sizeof(pBuf);
	}
};


#endif /* QUEUE_H_ */
