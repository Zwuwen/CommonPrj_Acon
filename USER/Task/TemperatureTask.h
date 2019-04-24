#ifndef __DEMOTASK_H
#define	__DEMOTASK_H

#include "AIA_ModuleCore.h"
#include "AIA_PID.h"

#define TEMPERATURE_OFFSET 0


/* Exported Types ------------------------------------------------------- */


/** 
  * @brief  
  */ 
typedef	struct _TEMPERATURE_PARAM
{
	int PerSisLen;
	int SetPointTemp[TOTAL_PID_NUMBER];
	int OffSet[TOTAL_PID_NUMBER];	
	
	int ReadingTemp[TOTAL_PID_NUMBER];
	
}_TEMPERATURE_PARAM;



/* Exported functions ------------------------------------------------------- */
int TemperatureTask_CmdProcess(AIAMODULE *module, int cmdword);
void TemperatureDataInit(void);
#endif















