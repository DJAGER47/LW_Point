#ifndef __DEFINE_H
#define __DEFINE_H

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>



typedef bool boolean;
typedef void (*_func)(void);


#define lenArr(x) (sizeof(x)/sizeof(x[0]))

#define __RamFunc __attribute__ ((section (".RamFunc")))

#define BOOT_SIZE	0x4000
#define PROGRAM_START_ADDRESS (FLASH_BASE + BOOT_SIZE)


#endif /* __DEFINE_H */

