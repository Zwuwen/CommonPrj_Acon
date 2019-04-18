/**
  ******************************************************************************
  * @file    AIA_Persistence.c
  * @author  Bowen.he
  * @version V2.0
  * @created   	16-April-2019
  * @lastModify 16-April-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
  
#include "AIA_Persistence.h" 
#include "IIC_Driver.h"
#include "Flash_Driver.h"

PERSISTENCE_PARAM PersistenceParams;

//PLL_SaveParams(&PersistenceParams, sizeof(PERSISTENCE_PARAM));

BOOL PLL_SaveParams(char *pbuf, int length)
{
#ifndef PARAM_SOURCE
#error "PARAM_SOURCE Not Defined!"
	
#elif (PARAM_SOURCE == I2C_2) || (PARAM_SOURCE == I2C_1)
	
	if(length>(EEROM_ADDR_END_MOTOR - EEROM_ADDR_START_MOTOR)) return FALSE;
	
	return SaveDataToI2cEprom((u8*)pbuf,EEROM_ADDR_START_MOTOR,length);	
	
#elif PARAM_SOURCE == FLASH_3
	
	if(length>(FLASH_ADDR_END_MOTOR-FLASH_ADDR_START_MOTOR)) return FALSE;
	
	return SaveDataToFlash((u32*)pbuf,FLASH_ADDR_START_MOTOR,length);	/*param must 4 byte aline*/	
#endif
	
}


BOOL PLL_ReadParams(char *pbuf, int length)
{
#ifndef PARAM_SOURCE
#error "PARAM_SOURCE Not Defined!"
#elif (PARAM_SOURCE == I2C_2) || (PARAM_SOURCE == I2C_1)

	if(length>(EEROM_ADDR_END_MOTOR - EEROM_ADDR_START_MOTOR)) return FALSE;
	
	return RestoreDataFromI2cEprom((u8*)pbuf,EEROM_ADDR_START_MOTOR,length);

#elif PARAM_SOURCE == FLASH_3
	
	if(length>(FLASH_ADDR_END_MOTOR-FLASH_ADDR_START_MOTOR)) return FALSE;
	
	return RestoreDataFromFlash((u32*)pbuf,FLASH_ADDR_START_MOTOR,length);
	
#endif
}

























