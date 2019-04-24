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
#include "TemperatureTask.h"
#include "string.h"

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
	BOOL PersistenceParamIsValid = FALSE;
	//TODO 是否有持久化的值，如果有，则使用持久化的值，如果没有，则使用默认的值。
	PersistenceParamIsValid	= PLL_ReadParams((char *)&PersistenceParams,sizeof(PersistenceParams));
		
	for(i=0;i<TOTAL_PID_NUMBER;++i)
	{
		if(TRUE == PersistenceParamIsValid) /* use Persistence Param */
		{
			///////////Temperature/////	
			SetPointTemp[i] = PersistenceParams.TargetValue[i];
			OffSet[i] = PersistenceParams.OffSet[i];	
			
			memcpy(&TemperParams.P, &PersistenceParams.PID[i], sizeof(LVPID[0]));
			
			///////////PID/////
			memcpy(&LVPID[i], &PersistenceParams.PID[i], sizeof(LVPID[0]));
			
			///////////core/////
			ModuleCore.address = PersistenceParams.moduleId;
		}	
		else /* use default Param */
		{
			///////////Temper/////	
			SetPointTemp[i] = 4000;
			OffSet[i] = 0;
			
			///////////PID/////		
			LVPID[i].flag.Bit.enablePIDTask = 0;  
			LVPID[i].Kp = 100;
			LVPID[i].Ti = 0;
			LVPID[i].Td = 0;
			LVPID[i].dt = 1;
			
			///////////core/////
			ModuleCore.address = 0x1b;
		}
		
		///////////PID/////
		LVPID[i].pSV = &SetPointTemp[i];
		LVPID[i].pPV = &ReadingTemp[i];	
	
		if(LVPID[i].flag.Bit.enablePIDTask == 1)
		{
			LVPID_Init_or_Reset(&LVPID[i]);
			LVPID[i].flag.Bit.enablePIDTask = 1;
			LVPID[i].flag.Bit.regulationOnce = 1; /*Regulate immediately*/
			LVPID[i].Count_dt = 0;
		}
		LVPID_SetGainAndDt(&LVPID[i], LVPID[i].Kp , LVPID[i].Ti ,LVPID[i].Td , LVPID[i].dt);	
	}	
	///////////PID/////
	LVPID[0].implementMV = implement_MV_Ch1;
	LVPID[1].implementMV = implement_MV_Ch2;
	LVPID[2].implementMV = implement_MV_Ch3;
	LVPID[3].implementMV = implement_MV_Ch4;
}

void PerparePersistenceData(void)
{
	///////////Temper/////
//PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	PerpareToPLL(PersistenceParams.AREA)
//	PersistenceParams.OffSet[0] = TempStruct.offset[0];
//	PersistenceParams.TargetValue[0] = TempStruct.TargetValue[0];
//	
//	//////////motor//////////////
//	PersistenceParams.Speed[0] = Motor.Speed[0];
	
	///////////////////////
	
}

//TODO  准备持久化数据。before  PLL_SaveParams
//从持久化数据中，分发   after PLL_ReadParams
