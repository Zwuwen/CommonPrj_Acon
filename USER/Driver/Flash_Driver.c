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
#include "Flash_Driver.h"
#include "string.h"
#include "AIA_Utilities.h"


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;



/**
  * @brief  从Flash中恢复数据
  * @param  None
  * @retval None
  */
BOOL RestoreDataFromFlash(u32 *pData,u32 SrcAddress,u32 Length)
{
	u32 i;
	
	if(FLASH_WRITTEN_SYMBOL != *((u32*)(FLASH_ADDR_END_MOTOR-4))) return FALSE;
		
	for(i=0;i<Length/4;i++)
	{				
		*(pData+i) = *((u32*)SrcAddress+i);	
	}	
	return TRUE;

}

BOOL SaveDataToFlash(u32 *pData,u32 DirAddress,u32 Length)
{
	u32 i;
	u32 EraseCounter = 0x00, NbrOfPage = 0, StartSectorAddress,EndSectorAddress,SectorMask;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;	 

#ifndef FLASH_PAGE_SIZE

#error "PARAM_SOURCE Not Defined!"

#elif (FLASH_PAGE_SIZE == 0x400) /*1k*/
	
	SectorMask = 0xFFFFFC00;
	
#elif (FLASH_PAGE_SIZE == 0x800) /*2k*/
	
	SectorMask = 0xFFFFF800;
	
#elif (FLASH_PAGE_SIZE == 0x1000) /*4k*/
	
	SectorMask = 0xFFFFF000;
#elif (FLASH_PAGE_SIZE == 0x2000) /*8k*/
	
	SectorMask = 0xFFFFE000;
	
#elif (FLASH_PAGE_SIZE == 0x4000) /*16k*/
	
	SectorMask = 0xFFFFC000;
	
#endif

	StartSectorAddress = DirAddress & SectorMask;
	EndSectorAddress	= (DirAddress+Length) & SectorMask;
	
	NbrOfPage = (EndSectorAddress - StartSectorAddress)/FLASH_PAGE_SIZE + 1;
	
	FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(StartSectorAddress + (FLASH_PAGE_SIZE * EraseCounter));
		if(FLASHStatus != FLASH_COMPLETE) return FALSE;
    }
	
	for(i=0;i<Length/4;i++) /*param must 4 byte aline*/
	{
		FLASHStatus = FLASH_ProgramWord(DirAddress+i*4, pData[i]);	
		if(FLASHStatus != FLASH_COMPLETE) 
		return FALSE;
	}

	FLASHStatus = FLASH_ProgramWord(FLASH_ADDR_END_MOTOR -4, FLASH_WRITTEN_SYMBOL);
	if(FLASHStatus != FLASH_COMPLETE) return FALSE;
	
    FLASH_Lock();
	return TRUE;			
}
