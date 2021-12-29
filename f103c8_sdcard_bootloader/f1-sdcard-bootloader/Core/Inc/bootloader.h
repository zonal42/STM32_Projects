/**
 *******************************************************************************
 * STM32 Bootloader Header
 *******************************************************************************
 * @author Ziya ÖNAL
 * @file   bootloader.h
 * @brief  This file contains the bootloader configuration parameters,
 *	       function prototypes and other required macros and definitions.

 *******************************************************************************
 * @copyright (c) 2020 Ziya ÖNAL.                    
 *******************************************************************************
 */

#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#define APP1_START (0x08005000)			//Origin + Bootloader size (20kB)
#define FLASH_BANK_SIZE (0X5800)		//22kB
#define FLASH_PAGE_SIZE_USER (0x400)	//1kB
#define CONF_FILENAME "app-demo.bin"
#define ERASE_FLASH_MEMORY "#$ERASE_MEM"
#define FLASHING_START "#$FLASH_START"
#define FLASHING_FINISH "#$FLASH_FINISH"
#define FLASHING_ABORT "#$FLASH_ABORT"
#define BOOT1_GPIO_Port BTN1_GPIO_Port
#define BOOT1_Pin BTN1_Pin
#include "main.h"

typedef enum
{
    JumpMode,
	FlashMode
} BootloaderMode;

typedef enum
{
    Unerased,
	Erased,
	Unlocked,
	Locked
} FlashStatus;

typedef void (application_t)(void);

typedef struct
{
    uint32_t		stack_addr;     // Stack Pointer
    application_t*	func_p;        // Program Counter
} JumpStruct;

uint32_t Flashed_offset;
FlashStatus flashStatus;

void bootloaderInit();
uint8_t flashWord(uint32_t word);
uint32_t readWord(uint32_t address);
void eraseMemory();
void unlockFlashAndEraseMemory();
void lockFlash();
void jumpToApp();
void deinitEverything();
uint8_t string_compare(char array1[], char array2[], uint16_t length);
uint8_t Bootloader_CheckSize(uint32_t appsize);

#endif /* __BOOTLOADER_H */
