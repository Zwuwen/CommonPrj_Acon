#ifndef __DEMOTASK_H
#define	__DEMOTASK_H

#include "AIA_ModuleCore.h"

#define TEMPERATURE_OFFSET 0

extern int ReadingTemp[4];
extern int SetPointTemp[4];





/* Exported functions ------------------------------------------------------- */
int TemperatureTask_CmdProcess(AIAMODULE *module, int cmdword);
void TemperatureDataInit(void);
#endif















