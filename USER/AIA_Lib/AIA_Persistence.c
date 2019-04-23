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


void PersistencDataInit(void)
{
	int i;
	
	//TODO 是否有持久化的值，如果有，则使用持久化的值，如果没有，则使用默认的值。
	
	for(i=0;i<TOTAL_PID_NUMBER;++i)
	{
		PersistenceParams.PID[i].flag.Bit.enablePIDTask = 0;  
	}
	
	PersistenceParams.PID[i].Kp = 100;
	PersistenceParams.PID[i].Ti = 0;
	PersistenceParams.PID[i].Td = 0;
	PersistenceParams.PID[i].dt = 1;
	
	PersistenceParams.moduleId = 0x1b;
	
	///////////Temper/////
	
	//////////motor//////////////
	
}


void PerparePersistenceData(void)
{
	///////////Temper/////
//PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	
//	PersistenceParams.OffSet[0] = TempStruct.offset[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	
//	//////////motor//////////////
//	PersistenceParams.Speed[0] = Motor.Speed[0];
	
	///////////////////////
	
}

//TODO  准备持久化数据。before  PLL_SaveParams
//从持久化数据中，分发   after PLL_ReadParams
