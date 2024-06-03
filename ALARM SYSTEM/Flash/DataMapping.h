#ifndef FLASH_DATA_MAPPING_H_
#define FLASH_DATA_MAPPING_H_

#include "stdint.h"
#include "stm32f1xx_hal.h"

#define NUMBER_OF_PAGES 64 // 64 pages for stm32f103c8t6 from 0-63 you can define your own for other F1 series
#define START_STORE_SECTOR 55 // my start storage sector 

#define FLASH_DRIVER_FIRST_PAGE_ALLOW_TO_BE_ERASED ((uint32_t)0x08000000) //FIRST_FLASH_PAGE_ALLOW_TO_ERASE
#define PERSISTENT_DATA_START ((uint32_t)(0x08000000 + FLASH_PAGE_SIZE*START_STORE_SECTOR))
#define FLASH_DRIVER_LAST_PAGE_ALLOW_TO_BE_ERASED   ((uint32_t)0x0800ffff)  //LAST_FLASH_PAGE_ALLOW_TO_ERASE

#endif // FLASH_DATA_MAPPING_H_
