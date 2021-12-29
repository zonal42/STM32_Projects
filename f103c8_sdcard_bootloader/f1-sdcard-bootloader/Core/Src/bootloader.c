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
#include "fatfs.h"
#include "fatfs_sd.h"
#include "main.h"

FATFS fs; // file system
FIL fil; //file

FRESULT fr;//to store the result
uint32_t data;// to store the data
char buffer[1024];

UINT br; //file read/write count

/* capacity related variables */

FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

int bufsize (char *buf)
{
int i=0;
while(*buf++!='\0')i++;
return i;
}



void bootloaderInit()
{
	uint32_t addr;
	UINT     num;

	Flashed_offset = 0;
	uint32_t cntr;
	flashStatus = Unerased;
	BootloaderMode bootloaderMode;
	if(HAL_GPIO_ReadPin(BOOT1_GPIO_Port, BOOT1_Pin) == GPIO_PIN_SET)
	{
		bootloaderMode = FlashMode;	


  
		 fr = f_mount(&fs, "", 0);
		 if(fr!=FR_OK) HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);


		/*take free space and total space in sd card*/
		 if(f_getfree("", &fre_clust, &pfs) != FR_OK)
		{
			HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
			Error_Handler();
		}
		else
		{
	   /*taking total and free spaces to variables*/
	     total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	     free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
		 
		 HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		 HAL_Delay(10);
		 HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		 HAL_Delay(90);
		 
		}

		
		
		
	}
	else bootloaderMode=JumpMode;

	if(bootloaderMode==JumpMode)
	{
		uint8_t emptyCellCount = 0;
		for(uint8_t i=0; i<10; i++)
			{
				if(readWord(APP1_START + (i*4)) == -1)
				emptyCellCount++;
			}

		if(emptyCellCount != 10)
		jumpToApp(APP1_START);
		else
		Error_Handler();

	}
	else if(bootloaderMode==FlashMode)
	{
		if(f_open(&fil, CONF_FILENAME, FA_READ)!=FR_OK)
		{
			Error_Handler();
		}
		if(Bootloader_CheckSize(f_size(&fil)) != 1)
		{
			Error_Handler();
		}
		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		HAL_Delay(100);
		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		HAL_Delay(100);
		unlockFlashAndEraseMemory();
		cntr = 0;
		HAL_FLASH_Unlock();
		do
		{

			f_read(&fil, &data, 4, &br);


			if(num)
			{
				if(flashWord(data)==1)
				{
					cntr++;
				}
				else
				{
					 f_close(&fil);
					 Error_Handler();
				}
				 if(cntr % 256 == 0)
				 {
					 HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin);
					 HAL_Delay(100);
					 HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
					 HAL_Delay(100);

				 }
			}

		} while((num!=0));
		lockFlash();
		f_close(&fil);

		/*Verification*/
		addr=APP1_START;
		cntr=0;
		do
		{
		data = 0xFFFFFFFF;
		fr = f_read(&fil, &data, 4, &num);
		if(num)
		{
			 if(*(uint32_t*)addr == (uint32_t)data)
            {
                addr += 4;
                cntr++;
            }
			else
			{
				Error_Handler();
			}
		}
		}while((fr == FR_OK) && (num > 0));

		fr=f_mount(NULL,"",1);
		if(fr==FR_OK)
		{
			HAL_GPIO_TogglePin(LED_B_GPIO_Port,LED_B_Pin|LED_G_Pin);
			HAL_Delay(1000);
			HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin|LED_G_Pin);
			HAL_Delay(1000);
		}

	}
	
}


uint8_t flashWord(uint32_t dataToFlash)
{

	if(flashStatus == Unlocked)
		{
		  volatile HAL_StatusTypeDef status;
		  uint8_t flash_attempt = 0;
		  uint32_t address;
		  do
		  {
			  address=APP1_START+Flashed_offset;
			  status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, dataToFlash);
			  flash_attempt++;
		  }while(status != HAL_OK && flash_attempt < 10 && dataToFlash == readWord(address));
		  if(status != HAL_OK)
		  	  {
		  		  Error_Handler();
		  	  }else
		  	  {//Word Flash Successful
		  		  Flashed_offset += 4;
		  		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		  		HAL_Delay(100);
		  		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		  		HAL_Delay(100);
				return 1;
		  	  }
		}

		return 0;

}


uint32_t readWord(uint32_t address)
{
	uint32_t read_data;
	read_data = *(uint32_t*)address;
	return read_data;
}


void eraseMemory()
{

	/* Unock the Flash to enable the flash control register access *************/
		while(HAL_FLASH_Unlock()!=HAL_OK)
			while(HAL_FLASH_Lock()!=HAL_OK);//Weird fix attempt

		/* Allow Access to option bytes sector */
		while(HAL_FLASH_OB_Unlock()!=HAL_OK)
			while(HAL_FLASH_OB_Lock()!=HAL_OK);//Weird fix attempt

		/* Fill EraseInit structure*/
		FLASH_EraseInitTypeDef EraseInitStruct;
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;

			EraseInitStruct.PageAddress = APP1_START;

		EraseInitStruct.NbPages = FLASH_BANK_SIZE/FLASH_PAGE_SIZE_USER;
		uint32_t PageError;

		volatile HAL_StatusTypeDef status_erase;
		status_erase = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

		/* Lock the Flash to enable the flash control register access *************/
		while(HAL_FLASH_Lock()!=HAL_OK)
			while(HAL_FLASH_Unlock()!=HAL_OK);//Weird fix attempt

		/* Lock Access to option bytes sector */
		while(HAL_FLASH_OB_Lock()!=HAL_OK)
			while(HAL_FLASH_OB_Unlock()!=HAL_OK);//Weird fix attempt

		if(status_erase != HAL_OK)
			Error_Handler();
		flashStatus = Erased;
		Flashed_offset = 0;
	}


void unlockFlashAndEraseMemory()
{
	/* Unock the Flash to enable the flash control register access *************/
	while(HAL_FLASH_Unlock()!=HAL_OK)
		while(HAL_FLASH_Lock()!=HAL_OK);//Weird fix attempt

	/* Allow Access to option bytes sector */
	while(HAL_FLASH_OB_Unlock()!=HAL_OK)
		while(HAL_FLASH_OB_Lock()!=HAL_OK);//Weird fix attempt

	if(flashStatus != Erased)
	{
		/* Fill EraseInit structure*/
		FLASH_EraseInitTypeDef EraseInitStruct;
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;

			EraseInitStruct.PageAddress = APP1_START;

		EraseInitStruct.NbPages = FLASH_BANK_SIZE/FLASH_PAGE_SIZE_USER;
		uint32_t PageError;

		volatile HAL_StatusTypeDef status_erase;
		status_erase = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

		if(status_erase != HAL_OK)
			Error_Handler();
	}

	flashStatus = Unlocked;
}


void lockFlash()
{
	/* Lock the Flash to enable the flash control register access *************/
	while(HAL_FLASH_Lock()!=HAL_OK)
		while(HAL_FLASH_Unlock()!=HAL_OK);//Weird fix attempt

	/* Lock Access to option bytes sector */
	while(HAL_FLASH_OB_Lock()!=HAL_OK)
		while(HAL_FLASH_OB_Unlock()!=HAL_OK);//Weird fix attempt

	flashStatus = Locked;
}


void jumpToApp(const uint32_t address)
{
	const JumpStruct* vector_p = (JumpStruct*)address;

	deinitEverything();

	/* let's do The Jump! */
    /* Jump, used asm to avoid stack optimization */
    asm("msr msp, %0; bx %1;" : : "r"(vector_p->stack_addr), "r"(vector_p->func_p));
}

void deinitEverything()
{
	//-- reset peripherals to guarantee flawless start of user application
	HAL_GPIO_DeInit(LED_R_GPIO_Port, LED_R_Pin|LED_G_Pin|LED_B_Pin);

	  __HAL_RCC_GPIOC_CLK_DISABLE();
	  __HAL_RCC_GPIOD_CLK_DISABLE();
	  __HAL_RCC_GPIOB_CLK_DISABLE();
	  __HAL_RCC_GPIOA_CLK_DISABLE();
	HAL_RCC_DeInit();
	HAL_DeInit();
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;
}

uint8_t Bootloader_CheckSize(uint32_t appsize)
{
    return ((FLASH_BASE + FLASH_BANK1_END - APP1_START) >= appsize) ? 1
                                                                : 0;
}



