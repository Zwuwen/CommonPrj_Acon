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
  * @brief  ��Flash�лָ�����
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
  * @brief  ��FLASH�б���һЩ������һ�����豸�����ϵ�������������ò�����ֱ�Ӵ�FLASH�лָ�����
  * @param  None
  * @retval None
  */
void SaveDataToFlash(void)
{
//	u16 i;
	FLASH_Unlock();

    //������Ҫ������ҳ��
  	NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    //������е�Falsh��־λ
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    //��ҳ��������Falsh 
    for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
    }

    //���ֶԵ�һ��Falsh ���д����
    Address = BANK1_WRITE_START_ADDR;			   

//	FLASHStatus = FLASH_ProgramWord(Address, Debug);	
	while(FLASHStatus != FLASH_COMPLETE);


	//�������һ���ֽڵ�����λ0x5a5a5a5a�������ж��Ƿ�������FLASH�д������
	//���һ�����ݱ���0x5a5a5a5a�����Ա�ֻ֤Ҫ���������ȷ�������Ĳ���һ����ȷ��������ʱ������Bank���Ա�һ�����
	FLASHStatus = FLASH_ProgramWord((BANK1_WRITE_END_ADDR-4), FLASH_WRITTEN_SYMBOL);
	while(FLASHStatus != FLASH_COMPLETE);

    //��������Falsh 
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


