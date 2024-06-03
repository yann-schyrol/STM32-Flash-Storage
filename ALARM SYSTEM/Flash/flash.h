/*
 * flash.h
 *
 *  Created on: May 28, 2024
 *      Author: Yann Schyrol Kiti
 */

#ifndef FLASH_H_
#define FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"



void write_flash_memory(uint32_t address, uint32_t* data, uint16_t num_of_words);
void read_flash_memory(uint32_t address, uint32_t* data, uint16_t num_of_words);



#ifdef __cplusplus
}
#endif

#endif /* FLASH_H_ */
