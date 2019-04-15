/**
  ******************************************************************************
  * @file    Flash_Driver.c 
  * @author  AIA Team
  * @version V1.1
  * @created    15-April-2019
  * @lastModify 
  * @brief   Flash Driver:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __FLASH_DRIVER_H
#define	__FLASH_DRIVER_H

#include "AIA_Utilities.h"

#define FLASH_WRITTEN_SYMBOL 0x5a5a5a5a

#define FLASH_ADDR_START_MOTOR ((u32)0x08019000)  /*254k*/
#define FLASH_ADDR_END_MOTOR ((u32)0x08019800)  /*256k*/

void SaveDataToFlash(void);
void RestoreDataFromFlash(void);
BOOL SaveDataTo2KFlash(u32 *pData, u32 IntLen, int addr);



#endif
