/*
 * flash.c
 *
 *  Created on: May 28, 2024
 *      Author: Yann Schyrol Kiti
 */


#include "flash.h"
#include "main.h"
#include "DataMapping.h"


static uint32_t GetPage(uint32_t adress){

	for(int i =0; i< NUMBER_OF_PAGES; i++){

		if((adress >= (FLASH_DRIVER_FIRST_PAGE_ALLOW_TO_BE_ERASED + FLASH_PAGE_SIZE*i)) && (adress < (FLASH_DRIVER_FIRST_PAGE_ALLOW_TO_BE_ERASED + FLASH_PAGE_SIZE*i+1))){

			return (FLASH_DRIVER_FIRST_PAGE_ALLOW_TO_BE_ERASED + FLASH_PAGE_SIZE*i); // return the start address of the memory sector

		}
	}

	return 0;
}
void read_flash_memory(uint32_t address, uint32_t* data, uint16_t num_of_words){

	for (int i = 0; i < num_of_words; i++) {
		*(data + i) = *((uint32_t*)(address+i*4));
	}
}


void write_flash_memory(uint32_t address, uint32_t* data, uint16_t num_of_words){

	static FLASH_EraseInitTypeDef EraseInitStruct;

	uint32_t pageError = 0;

	/* We must first unlock the flash to enable flash control reg*/
	HAL_FLASH_Unlock();

	/* Erase the flash area/page */
	uint32_t  startAddr = GetPage(address);

	/* fill erase init structure */
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress =  startAddr;
	EraseInitStruct.NbPages  = 1 + num_of_words*4/FLASH_PAGE_SIZE;

	HAL_FLASHEx_Erase(&EraseInitStruct, &pageError);

	int i = 0;

	while(i<num_of_words){


		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i*4 , data[i]);

		i++;
	}



	/*
	 * Lock memory back after writing to flash
	 */

	HAL_FLASH_Lock();

}

