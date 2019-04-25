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
#ifndef __AIA_PERSISTENCE_H
#define	__AIA_PERSISTENCE_H

#include "AIA_Utilities.h"
#include "AIA_PID.h"

#define TEMP_PERSISTENCE_LENGTH  32
#define PID_PERSISTENCE_LENGTH  5*4*TOTAL_PID_NUMBER
#define MOTOR_PERSISTENCE_LENGTH  20

/** 
  * @brief  
  */ 
typedef	struct _PERSISTENCE_PARAM
{
	char moduleId;
	char dummy[3];
	
	char temp[TEMP_PERSISTENCE_LENGTH];
	char pid[PID_PERSISTENCE_LENGTH];
	char motor[MOTOR_PERSISTENCE_LENGTH];
	
}PERSISTENCE_PARAM;

extern PERSISTENCE_PARAM PersistenceParams;


/* Exported functions ------------------------------------------------------------*/
BOOL PLL_SaveParams(char *pbuf, int length);
BOOL PLL_ReadParams(char *pbuf, int length);
void DistributePersistencData(void);
void PerparePersistenceData(void);

#endif
































