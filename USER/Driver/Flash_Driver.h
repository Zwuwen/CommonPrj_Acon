/**
  ******************************************************************************
  * @file    Flash_Driver.c 
  * @author  Xiufeng.Zhu
  * @version V1.0
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

//#define FLASH_ADDR_START_MOTOR ((u32)0x08019000)  /*254k*/
//#define FLASH_ADDR_END_MOTOR ((u32)0x08019800)  /*256k*/

#define FLASH_ADDR_START_MOTOR ((u32)0x08019000)  /*254k*/
#define FLASH_ADDR_END_MOTOR ((u32)0x0801A800)  /*260k*/

#define FLASH_PAGE_SIZE    0x800  

BOOL SaveDataToFlash(u32 *pData,u32 DirAddress,u32 Length);
BOOL RestoreDataFromFlash(u32 *pData,u32 SrcAddress,u32 Length);
BOOL SaveDataTo2KFlash(u32 *pData, u32 IntLen, int addr);


#endif
