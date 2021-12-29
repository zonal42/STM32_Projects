/**
 *******************************************************************************
 * STM32 Bootloader Source
 *******************************************************************************
 * @author Ziya ÖNAL
 * @file   bootloader.c
 * @brief  This file contains the functions of the bootloader. The bootloader
 *	       implementation uses the official HAL library of ST.

 *******************************************************************************
 * @copyright (c) 2020 Ziya ÖNAL.                    
 *******************************************************************************
 */
 /* Includes ------------------------------------------------------------------*/
#include "bootloader.h"

void bootloaderInit()
{
	
	Flashed_offset = 0;
	flashStatus = Unerased;
	BootloaderMode bootloaderMode;
	if(HAL_GPIO_ReadPin(BOOT1_GPIO_Port, BOOT1_Pin) == GPIO_PIN_SET)
	{
		bootloaderMode = FlashMode;	
		for(uint8_t i=0; i<5; i++)
		{
			HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin|LED_G_Pin);
			HAL_Delay(10);
			HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_R_Pin|LED_G_Pin);
			HAL_Delay(90);
		}

  
		 fresult = f_mount(&fs, "", 0);
		 if(fresult!=FR_OK) HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);


		/*take free space and total space in sd card*/
		 if(f_getfree("", &fre_clust, &pfs) != FR_OK)
		{
			HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		}
		else
		{
	   /*taking total and free spaces to variables*/
	     total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	     free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
		 
		 HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin|LED_G_Pin);
		 HAL_Delay(10);
		 HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_R_Pin|LED_G_Pin);
		 HAL_Delay(90);
		 
		}

		
		
		
	}
	
	
}