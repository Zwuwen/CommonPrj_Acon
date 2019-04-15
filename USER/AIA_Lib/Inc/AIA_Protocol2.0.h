/**
  ******************************************************************************
  * @file    AIA_Protocol2.0.c 
  * @author  Bowen.He
  * @version V2.0
  * @created    15-April-2019
  * @lastModify 
  * @brief   AIA_Protocol 2.0:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************
*/ 

#ifndef __AIA_PROTOCOL_H
#define	__AIA_PROTOCOL_H

#include "stm32f10x.h"



/* Exported functions ------------------------------------------------------------*/
int ParseCmdParam(char *cmd, int *val, int num);
void ResponseCmdByCan(int ret);

#endif
