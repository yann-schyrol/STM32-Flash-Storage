/*Flash Data Storage Guide

This guide explains how to integrate flash data storage into your STM32 project. The code provided demonstrates basic read and write operations for flash memory.

Prerequisites
STM32 development environment (STM32CubeIDE or similar)
Basic knowledge of C programming and STM32 HAL library
Include the flash.h header file in your project
Flash Data Storage Integration Steps
Step 1: Create DataMapping.h
Create a DataMapping.h file to define the start address and size for your flash memory data. Adjust these values according to your microcontroller's memory map:

DataMapping.h

#ifndef DATAMAPPING_H
#define DATAMAPPING_H

#define FLASH_START_ADDRESS 0x0800F000 // Example address, adjust as needed
#define FLASH_DATA_SIZE 3 // Number of 32-bit words to store

#endif // DATAMAPPING_H
Step 2: Include Necessary Headers
In your main.c, include the flash.h and DataMapping.h header files:


#include "flash.h"
#include "DataMapping.h"
Step 3: Declare Variables for Flash Data
Declare a buffer to store data that you want to write to or read from flash memory:


uint32_t flashData[DATA_SIZE] = {0};
Step 4: Implement Flash Read and Write Functions
Ensure the flash.h and flash.c files contain the necessary functions to read from and write to flash memory.

flash.h

#ifndef FLASH_H
#define FLASH_H

#include "stm32f1xx_hal.h"

void write_flash_memory(uint32_t startAddress, uint32_t *data, uint16_t length);
void read_flash_memory(uint32_t startAddress, uint32_t *data, uint16_t length);

#endif // FLASH_H

flash.c
#include "flash.h"

void write_flash_memory(uint32_t startAddress, uint32_t *data, uint16_t length) {
    HAL_FLASH_Unlock();
    for (uint16_t i = 0; i < length; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddress + i * 4, data[i]);
    }
    HAL_FLASH_Lock();
}

void read_flash_memory(uint32_t startAddress, uint32_t *data, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        data[i] = *(__IO uint32_t *)(startAddress + i * 4);
    }
}
Step 5: Initialize Flash Data
During system initialization, read the stored values from flash memory:


void System_Init(void) {
    // Initialize parameters
    read_flash_memory(FLASH_START_ADDRESS, flashData, FLASH_DATA_SIZE);

    // Use the flashData values as needed in your application
    // Example:
    // uint32_t myParameter = flashData[0];
}
Step 6: Save Data to Flash Memory
Whenever you need to save data to flash memory, update the flashData array and call the write_flash_memory function:


void Save_Data_To_Flash(void) {
    // Update flashData array with new values
    flashData[0] = newValue1;
    flashData[1] = newValue2;
    flashData[2] = newValue3;

    // Write updated data to flash memory
    write_flash_memory(FLASH_START_ADDRESS, flashData, FLASH_DATA_SIZE);
}
Example Usage in main.c
Here is a simplified example of how to use the flash read and write functions in your main.c:

#include "main.h"
#include "flash.h"
#include "DataMapping.h"

uint32_t flashData[DATA_SIZE] = {0};

void System_Init(void) {
    // Initialize system and read flash data
    HAL_Init();
    SystemClock_Config();
    read_flash_memory(FLASH_START_ADDRESS, flashData, FLASH_DATA_SIZE);

    // Use flashData as needed
    uint32_t parameter1 = flashData[0];
    uint32_t parameter2 = flashData[1];
    uint32_t parameter3 = flashData[2];
}

void Save_Data_To_Flash(void) {
    // Update flashData with new values
    flashData[0] = 1234;
    flashData[1] = 5678;
    flashData[2] = 91011;

    // Write updated data to flash memory
    write_flash_memory(FLASH_START_ADDRESS, flashData, FLASH_DATA_SIZE);
}

int main(void) {
    System_Init();

    while (1) {
        // Your main loop code

        // Save data to flash when needed
        if (/* condition to save data */) {
            Save_Data_To_Flash();
        }
    }
}
Conclusion
By following these steps, you can integrate flash data storage into your STM32 project. This integration allows you to store critical data that persists across power cycles, ensuring your system retains its configuration and state.
