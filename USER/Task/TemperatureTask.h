#ifndef __DEMOTASK_H
#define	__DEMOTASK_H

#include "AIA_ModuleCore.h"
#include "AIA_PID.h"

#define TEMPERATURE_OFFSET 0

extern int SetTemp[TOTAL_PID_NUMBER];
extern int OffSet[TOTAL_PID_NUMBER];	
extern int ReadingTemp[TOTAL_PID_NUMBER];

/* Exported Types ------------------------------------------------------- */


/* Exported functions ------------------------------------------------------- */
int TemperatureTask_CmdProcess(AIAMODULE *module, int cmdword);
void TemperatureData_FromPersistence(char *src);
int PerpareToTemperatureArea(char *dest);
#endif















