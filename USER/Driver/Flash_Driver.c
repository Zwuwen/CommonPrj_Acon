#include "Flash_Driver.h"
#include "string.h"


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/*64K 0X10000*/
#define FLASH_PAGE_SIZE    ((u16)0x400)    //1024
#define BANK1_WRITE_START_ADDR  ((u32)0x0800C000)
#define BANK1_WRITE_END_ADDR    ((u32)0x0800C400)


u32 EraseCounter = 0x00, Address = 0x00;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;

u8 NbrOfPage = 0x00;



/**
  * @brief  从Flash中恢复数据
  * @param  None
  * @retval None
  */
void RestoreDataFromFlash(void)
{
	Address = BANK1_WRITE_START_ADDR;
	if(FLASH_WRITTEN_SYMBOL == *((u32*)(BANK1_WRITE_END_ADDR-4)))
	{
//		Debug = *((u32*)(Address));	

	}		
}





/**
  * @brief  在FLASH中保存一些参数，一遍在设备重新上电后无需重新设置参数，直接从FLASH中恢复数据
  * @param  None
  * @retval None
  */
void SaveDataToFlash(void)
{
//	u16 i;
	FLASH_Unlock();

    //计算需要擦除的页数
  	NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    //清楚所有的Falsh标志位
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    //按页擦除整块Falsh 
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    //按字对第一块Falsh 编程写数据
    Address = BANK1_WRITE_START_ADDR;			   

//	FLASHStatus = FLASH_ProgramWord(Address, Debug);	
	while(FLASHStatus != FLASH_COMPLETE);


	//保存最后一个字节的数据位0x5a5a5a5a，用于判断是否曾经在FLASH中存过数据
	//最后一个数据保存0x5a5a5a5a，可以保证只要这个数据正确，其他的参数一定正确，擦除的时候整个Bank都对被一起擦除
	FLASHStatus = FLASH_ProgramWord((BANK1_WRITE_END_ADDR-4), FLASH_WRITTEN_SYMBOL);
	while(FLASHStatus != FLASH_COMPLETE);

    //重新锁定Falsh 
    FLASH_Lock();
}



BOOL SaveDataTo2KFlash(u32 *pData, u32 IntLen, int addr)
{
	u32 i;
	u32 Address;
	volatile FLASH_Status FLASHStatus;	 


	Address = 0x00;
	FLASHStatus = FLASH_COMPLETE;	

	FLASH_Unlock();

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASHStatus = FLASH_ErasePage( addr );

    Address =  addr;
	
	for(i=0;i<IntLen;i++)
	{
		FLASHStatus = FLASH_ProgramWord(Address+4*(i+1),*((u32*)pData+i));	
		while(FLASHStatus != FLASH_COMPLETE);
	}	
	
	Address = addr + 2044;
	FLASHStatus = FLASH_ProgramWord(Address, FLASH_WRITTEN_SYMBOL);	
	while(FLASHStatus != FLASH_COMPLETE){;}
	
    FLASH_Lock();
	return TRUE;
}


