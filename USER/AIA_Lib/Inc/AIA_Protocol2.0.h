#ifndef __VARIABLE_H
#define	__VARIABLE_H

#include "stm32f10x.h"




int ParseCmdParam(char *cmd, int *val, int num);
void ResponseCmdByCan(int ret);

#endif
