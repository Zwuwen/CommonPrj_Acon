/**
  ******************************************************************************
  * @file    AIA_Bootload.c 
  * @author  Bowen.He
  * @version V1.1
  * @created    20-November-2015
  * @lastModified 09-January-2017
  * @brief   AIA BOOTLOAD:  Update the Userapp or the first 8kb Bootload
  ******************************************************************************
  * @attention
  *
  * A typical AIA BOOTLOAD contains 2 files. There are :
  *	aia_bootload.c:
  * aia_bootload.h:
  * The update processing using the Extended Frame.
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#include "AIA_Bootload.h"
#include "AIA_Utilities.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"
#include "stdio.h"

BOOTFLAG BOOTFlag;
CMDTYPE CmdType;
u32 Erase_Start, Erase_Size;
u32 Write_Addr, Write_Data;
u32 Chunk_Addr;	
u16 Chunk_Size, Chunk_Seq;
u32 ChunkEnd1, ChunkEnd2;
u32 Run_Addr, Run_Check;
u32 Read_Addr;
u32 Go_Addr;

u32 TimeoutCount;

u32 APPCode[512]; 		/*pagesize/4*/
u32 ChunkSumXor;  		/*和校验的异或*/
u32 ChunkReadIndex; 	/*读取索引*/
u32 ChunkSum; 			/*求和校验*/
u16 CurrentSeq; 		/*当前序列*/
u32 DecryptKey;

void Bootload_Init(int deviceId)
{
	BOOTFlag.Allbits = 0x0000;
	DecryptKey = ((deviceId<<24)| (deviceId<<16)| (deviceId<<8)| deviceId)
				 ^ ((u32)(('A'<<24)|('C'<<16)|('O'<<8)|'N'));

}


void Bootload_CanInit(int deviceId)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	

	__disable_irq();
	
  /*CAN 寄存器初始化*/

	CAN_DeInit(CAN1);  


    CAN_StructInit(&CAN_InitStructure);

    /*CAN 初始化*/
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;	
    CAN_InitStructure.CAN_RFLM = DISABLE;	
    CAN_InitStructure.CAN_TXFP = ENABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; 
    CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_16tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 18;

	CAN_Init(CAN1,&CAN_InitStructure); 

    
	
	/*只接受6个type类型，防止总线上数据干扰，特别是泵的指令的干扰*/
    CAN_FilterInitStructure.CAN_FilterNumber 		= 	0;
    CAN_FilterInitStructure.CAN_FilterMode 			= 	CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale 		= 	CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh   		= 	(((u32)((CMD1_CHUNK_START<<8) | deviceId)<<3)&0xffff0000)>>16;	
	CAN_FilterInitStructure.CAN_FilterIdLow   		= 	(((u32)((CMD1_CHUNK_START<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  	= 	(((u32)((CMD2_CHUNK_WRITE<<8) | deviceId)<<3)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   	= 	(((u32)((CMD2_CHUNK_WRITE<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 	0;
    CAN_FilterInitStructure.CAN_FilterActivation = 		ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
	
    CAN_FilterInitStructure.CAN_FilterNumber 		= 	1;
    CAN_FilterInitStructure.CAN_FilterMode 			= 	CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale 		= 	CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh   		= 	(((u32)((CMD3_CHUNK_END<<8) | deviceId)<<3)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterIdLow   		= 	(((u32)((CMD3_CHUNK_END<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  	= 	(((u32)((CMD4_GET_VERSION<<8) | deviceId)<<3)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   	= 	(((u32)((CMD4_GET_VERSION<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 	0;
    CAN_FilterInitStructure.CAN_FilterActivation = 		ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
	
    CAN_FilterInitStructure.CAN_FilterNumber 		= 	2;
    CAN_FilterInitStructure.CAN_FilterMode 			= 	CAN_FilterMode_IdList;
    CAN_FilterInitStructure.CAN_FilterScale 		= 	CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh   		= 	(((u32)((CMD5_COMPLETE<<8) | deviceId)<<3)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterIdLow   		= 	(((u32)((CMD5_COMPLETE<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh  	= 	(((u32)((CMD0_RESERVE<<8) | deviceId)<<3)&0xffff0000)>>16;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow   	= 	(((u32)((CMD0_RESERVE<<8) | deviceId)<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xffff;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 	0;
    CAN_FilterInitStructure.CAN_FilterActivation = 		ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	
	__enable_irq();	
}





u8 WriteBootCodeIntoFlash(int deviceId)
{
	u32 boot;

	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	FLASH_Unlock();

    //清楚所有的Falsh标志位
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASHStatus = FLASH_ErasePage(FLASH_ADDR_BOOTCODE);
	if(FLASHStatus != FLASH_COMPLETE) {FLASH_Lock(); return 0;}

 	boot = ((deviceId<<24)| (deviceId<<16)| (deviceId<<8)| deviceId);
	boot ^= ((u32)(('A'<<24)|('C'<<16)|('O'<<8)|'N'));
	FLASHStatus = FLASH_ProgramWord(FLASH_ADDR_BOOTCODE, boot);
	if(FLASHStatus != FLASH_COMPLETE) {FLASH_Lock(); return 0;}

    FLASH_Lock();

	return 1;
}




/**
  * @brief 
  * @param 
  * @retval 
  */
#define __IO volatile
void CAN_BOOT_JumpToApplication(u32 Addr)
{
	pFunction Jump_To_Application;
	__IO u32 JumpAddress; 
	/* Test if user code is programmed starting from address "ApplicationAddress" */
	if (((*(__IO u32*)Addr) & 0x2FFE0000 ) == 0x20000000)
	{ 
	  /* Jump to user application */
	  JumpAddress = *(__IO u32*) (Addr + 4);
	  Jump_To_Application = (pFunction) JumpAddress;
		__disable_irq();
		FLASH_Lock();
	  /* Initialize user application's Stack Pointer */
	  
	  __set_MSP(*(__IO u32*) Addr);

	  Jump_To_Application();
	}
}



#if(BT_LEDFLASH_ENABLE == 1)
void BTLED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( BT_RCC_LEDGPIO, ENABLE);  

	GPIO_InitStructure.GPIO_Pin = BT_LEDPIN;	
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(BT_LEDGPIO, &GPIO_InitStructure);	
}
#endif


/**
  * @brief 
  * @param  
  * @retval
  */

void SendCanResp(int addr, CMDTYPE Cmd, u8 Status)
{
	u8 TransmitMailbox = 0;	
	CanTxMsg txMsg;
	txMsg.StdId =  0;
	txMsg.ExtId = (addr | ((u32)Cmd<<8));
	txMsg.RTR 	= CAN_RTR_DATA;	
	txMsg.IDE 	= CAN_ID_EXT;
	txMsg.DLC   = 1;
	txMsg.Data[0] = Status;
	
	TransmitMailbox = CAN_Transmit(CAN1, &txMsg);
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK);


}


/**
  * @brief  将程序数据写入指定Flash地址
  * @param  Address 起始地址
  * @param  Data 准备写入的数据
  * @retval 数据写入状态，0-写入数据成功，1-写入数据失败
  */
u8 CAN_BOOT_ProgramPage(u32 Addr, u32 Size)
{
	u32 i = 0;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	if(FLASH_COMPLETE != FLASH_ErasePage(Addr)) return 0;
	
	/* Program Flash Bank1 */
	while((i < Size) && (FLASHStatus == FLASH_COMPLETE))
	{
		FLASHStatus = FLASH_ProgramWord(Addr + i*4 , APPCode[i]);
		i++;
	}
	if(i == Size)
		return 1;
	else
		return 0;
}




/**
  * @brief  处理CAN命令。
  * @param  。
  * @retval 。
  */
void ProcessCanCMD(AIAMODULE *module)
{
	u8 CmdExcuStatus = 0;
	u8 AnswerStr[8];

	if(!BOOTFlag.Bit.CanNewFrame) return; 

	switch(CmdType)
	{
		case CMD1_CHUNK_START:
			if((Chunk_Addr >= FLASH_ADDR_BOOTSTART) && (Chunk_Addr <= FLASH_ADDR_APPLICATION) && (Chunk_Size <= 512))
			{
				CmdExcuStatus = 1;	
				BOOTFlag.Bit.isReceiveChunk = 1;
				ChunkReadIndex = 0;
				ChunkSum = 0;		
			}
			SendCanResp(module->address, CMD1_CHUNK_START, CmdExcuStatus);			
		break;

		case CMD3_CHUNK_END:
			CmdExcuStatus = 2;
			if(ChunkSum == (ChunkEnd1 ^ ChunkEnd2))	/*校验匹配，可以开始写入到flash里了*/
			{/*要确保Chunk_Addr之类的在使用前不被修改*/
				BOOTFlag.Bit.isReceiveChunk = 0;  /*已经完成当前任务*/
				CmdExcuStatus = 3;
				if(CAN_BOOT_ProgramPage(Chunk_Addr,Chunk_Size))
				{
					CmdExcuStatus = 1;
					CurrentSeq++;
					ChunkSumXor ^= ChunkSum;
		
				}									
			}
			SendCanResp(module->address, CMD3_CHUNK_END, CmdExcuStatus);
		break;
		
		case CMD4_GET_VERSION:
			CmdExcuStatus = 1;
			BOOTFlag.Bit.isReceiveChunk = 0;
			CurrentSeq = 0;
			AnswerStr[0] = CmdExcuStatus;
			AnswerStr[1] = BT_FLASH_PAGESIZE/1024 +'0'; /*Page Size*/
			AnswerStr[2] = 'K';
			AnswerStr[3] = 'B';  /*接下去要更新Bootload*/
			AnswerStr[4] = '\0';				
			CANSendStringExt((char *)AnswerStr, (module->address | ((u16)CMD4_GET_VERSION<<8)));
		break;
		
		case CMD5_COMPLETE:
			if(ChunkSumXor == Run_Check)
			{
				if ( (U32_VALUE_BOOTHEAD & 0x2FFE0000)  == 0x20000000)
				{				
					FLASH_Lock();
					CmdExcuStatus = 1;
					BOOTFlag.Bit.UpdateComplete = 1;
				}			
			}
			SendCanResp(module->address, CMD5_COMPLETE, CmdExcuStatus);
			FLASH_Lock();

			__disable_fault_irq();  
			NVIC_SystemReset();	

			
		break;
		
		default:
		break;
	}
	BOOTFlag.Bit.CanNewFrame = 0; 	
}


int BootLoad_ServerInIrq(CanRxMsg* RxMsg, int deviceId)
{
	u32 DecryptedByte;
    if ((BOOTFlag.Bit.EnterUpdateMode == 1)  && (RxMsg->ExtId&0x7f) == deviceId) 
    {
		if(BOOTFlag.Bit.CanNewFrame) return 1;   //正在处理
		CmdType = (CMDTYPE)((RxMsg->ExtId>>8)&0x000f);
		
		switch(CmdType)
		{
			case CMD1_CHUNK_START:
				if(BOOTFlag.Bit.isReceiveChunk)	break;
				Chunk_Seq = U32_CANFRAME67;
				if(Chunk_Seq != CurrentSeq) break; /*序列号不匹配*/
				Chunk_Addr = U32_CANFRAME0123;
				Chunk_Size = U32_CANFRAME45;
				BOOTFlag.Bit.CanNewFrame = 1;
			break;

			case CMD2_CHUNK_WRITE:
				if(!BOOTFlag.Bit.isReceiveChunk) break;
				if(ChunkReadIndex > (Chunk_Size-1))	 /*满了*/
				{
					break;
				} /*ChunkReadIndex 最大到1023*/
				DecryptedByte = U32_CANFRAME0123 ^ DecryptKey;
				APPCode[ChunkReadIndex++] = DecryptedByte;
				ChunkSum += DecryptedByte;
				if(ChunkReadIndex > (Chunk_Size-1))
				{
					break;
				} /*ChunkReadIndex 最大到1023*/
				DecryptedByte = U32_CANFRAME4567 ^ DecryptKey;
				APPCode[ChunkReadIndex++] = DecryptedByte;
				ChunkSum += DecryptedByte;
				 
			break;
			
			case CMD3_CHUNK_END:
				if(BOOTFlag.Bit.isReceiveChunk)
				{
					ChunkEnd1 = U32_CANFRAME0123;
					ChunkEnd2 = U32_CANFRAME4567;
					BOOTFlag.Bit.CanNewFrame = 1;
				}
			break;
			
			case CMD4_GET_VERSION:
				BOOTFlag.Bit.CanNewFrame = 1;
			break;			
			
			case CMD5_COMPLETE:
				Run_Addr = U32_CANFRAME0123;
				Run_Check = U32_CANFRAME4567;
				if(Run_Addr>= FLASH_ADDR_BOOTSTART)
				{
					BOOTFlag.Bit.CanNewFrame = 1;
				}
			break;
			
		default:
			break;
		} 
		return 1;
	}
	return 0;


}

/**
  * @brief  Update App
  * @param  
  * @retval res
  */
int YK_Process(AIAMODULE *module) /* "BOOTLOAD" */
{	
	int bootcode;
		
	CHECK_PARAM_NUMBER(1);
	
	bootcode = ((module->address<<24) | (module->address<<16) | (module->address<<8) | module->address);
	
	if(bootcode == module->recvParams[0])	
	{
		if(WriteBootCodeIntoFlash(module->address))
		{				
			PrepareResponseBuf(module, "%d", EXECUTE_SUCCESS);
			SendModuleResponse(module);
			
			/* reset*/
			__disable_fault_irq();  
			NVIC_SystemReset();	
			
			return RESPONSE_IN_PROCESS;
		}
	}
	return ERR_PARAM;
}




/**
  * @brief  Update Bootload.
  * @param  
  * @retval res
  */
int YM_Process(AIAMODULE *module) /* "UPDATEBT" */
{
	int bootcode;
	
	CHECK_PARAM_NUMBER(1);

	bootcode = ((module->address<<24) | (module->address<<16) | (module->address<<8) | module->address);
	BOOTFlag.Allbits = 0x0000;
	
	if(bootcode == module->recvParams[0])
	{
		PrepareResponseBuf(module, "%d", EXECUTE_SUCCESS);
		SendModuleResponse(module);
		
		Bootload_CanInit(module->address);
		
		BOOTFlag.Bit.EnterUpdateMode = 1;
		
		ChunkSumXor = 0;
		CurrentSeq = 0;

		FLASH_Unlock();  /**/
	
		while(1)
		{
			if(BOOTFlag.Bit.CanNewFrame)
			{
				ProcessCanCMD(module);
				BOOTFlag.Bit.CanNewFrame = 0;	
			}		
		}		
	}
	return ERR_PARAM;
}



int BOOT_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(YK);	/* "BOOTLOAD" */
		CASE_REGISTER_CMD_PROCESS(YM);	/* "UPDATEBT" */
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}










