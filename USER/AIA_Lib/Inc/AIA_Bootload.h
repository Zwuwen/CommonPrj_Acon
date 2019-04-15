/**
  ******************************************************************************
  * @file    aia_bootload.h
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

#ifndef __AIA_BOOTLOAD_H
#define	__AIA_BOOTLOAD_H

#include "stm32f10x.h"




/*-------Bootload Config---------*/
#define BT_FLASH_PAGESIZE 		2048	/*Pagesize = 1024 or 2048*/
#define BT_WAIT_TIMEOUT_S 		1500 /*10S = 10ms * 1000 */
#define BT_PARAM_SOURCE			I2C_1	/*I2C_1 I2C_2 FLASH*/
#define BT_STM32LIB_VER			VERSION_3_5

#define BT_LEDFLASH_ENABLE 		0
#define BT_RCC_LEDGPIO			RCC_APB2Periph_GPIOC
#define BT_LEDGPIO 				GPIOC
#define BT_LEDPIN 				GPIO_Pin_13



#define BT_DONTBT_FLAG			0x12345678




#define LED_RUN(a);	if (a)	\
					{GPIO_SetBits(BT_LEDGPIO,BT_LEDPIN);}\
					else		\
					{GPIO_ResetBits(BT_LEDGPIO,BT_LEDPIN);}





/*
64K 	0x10000
128K	0x20000
256K	0x40000
*/
#if(BT_FLASH_PAGESIZE == 2048)
#define FLASH_ADDR_BOOTCODE     ((u32)0x0801FFFC)
#define FLASH_ADDR_APPLICATION  ((u32)0x08002000)
#define FLASH_ADDR_BOTTOM		((u32)0x08020000)
#elif(BT_FLASH_PAGESIZE == 1024)
#define FLASH_ADDR_BOOTCODE     ((u32)0x0800FFFC)
#define FLASH_ADDR_APPLICATION  ((u32)0x08002000)
#define FLASH_ADDR_BOTTOM		((u32)0x08010000)
#endif



#define FLASH_ADDR_APPLICATION  ((u32)0x08002000)
#define FLASH_ADDR_BOOTSTART 	((u32)0x08000000)



#define U32_VALUE_BOOTCODE		(*((u32*)FLASH_ADDR_BOOTCODE))
#define U32_VALUE_APPHEAD		(*((u32*)FLASH_ADDR_APPLICATION))

#define U32_VALUE_BOOTHEAD		(*((u32*)FLASH_ADDR_BOOTSTART))

typedef  void (*pFunction)(void);


/** 
  * @brief  Global Flag bit define  
  */ 
typedef union
{
	struct
	{
		unsigned LedRUNTurnOn			:1; 
		unsigned LEDLast				:1;
		unsigned LEDNow					:1; 
		unsigned CanNewFrame			:1;
		unsigned isReceiveChunk			:1; 
		unsigned bit5					:1;
		unsigned UpdateComplete			:1; 
		unsigned EnterUpdateMode		:1;
	}Bit;
	u32 Allbits;
}BOOTFLAG;
extern BOOTFLAG BOOTFlag;


/** 
  * @brief  CMD TYPE
  */ 
typedef enum  
{
	CMD0_RESERVE = 0, /*000*/
	CMD1_CHUNK_START, /*001*/
	CMD2_CHUNK_WRITE, /*010*/
	CMD3_CHUNK_END,   /*011*/
	CMD4_GET_VERSION, /*100*/
	CMD5_COMPLETE,    /*101*/
}CMDTYPE;
extern CMDTYPE CmdType;


#define U32_CANFRAME0123 ((RxMsg->Data[0]<<24)|(RxMsg->Data[1]<<16)|(RxMsg->Data[2]<<8)|(RxMsg->Data[3]))
#define U32_CANFRAME4567 ((RxMsg->Data[4]<<24)|(RxMsg->Data[5]<<16)|(RxMsg->Data[6]<<8)|(RxMsg->Data[7]))
#define U32_CANFRAME01 ((RxMsg->Data[0]<<8)|(RxMsg->Data[1]))
#define U32_CANFRAME23 ((RxMsg->Data[2]<<8)|(RxMsg->Data[3]))
#define U32_CANFRAME45 ((RxMsg->Data[4]<<8)|(RxMsg->Data[5]))
#define U32_CANFRAME67 ((RxMsg->Data[6]<<8)|(RxMsg->Data[7]))


#define CURRENTCMDTYPE (u8)(ModuleAdress | (CmdType<<4))
#define CMD0_RESP_ID  (u8)(ModuleAdress | (CMD0_RESERVE<<4))
#define CMD1_RESP_ID  (u8)(ModuleAdress | (CMD1_CHUNK_START<<4))
#define CMD2_RESP_ID  (u8)(ModuleAdress | (CMD2_CHUNK_WRITE<<4))
#define CMD3_RESP_ID  (u8)(ModuleAdress | (CMD3_CHUNK_END<<4))
#define CMD4_RESP_ID  (u8)(ModuleAdress | (CMD4_WRITE_ONE_U32<<4))
#define CMD5_RESP_ID  (u8)(ModuleAdress | (CMD5_READ_ONE_U32<<4))
#define CMD6_RESP_ID  (u8)(ModuleAdress | (CMD6_ERASE<<4))
#define CMD7_RESP_ID  (u8)(ModuleAdress | (CMD7_GET_VERSION<<4))
#define CMD8_RESP_ID  (u8)(ModuleAdress | (CMD8_RUN<<4))


extern CMDTYPE CmdType;
extern u32 Erase_Start, Erase_Size;
extern u32 Write_Addr, Write_Data;
extern u32 Chunk_Addr;
extern u16 Chunk_Size, Chunk_Seq;
extern u32 ChunkEnd1, ChunkEnd2;
extern u32 Run_Addr, Run_Check;
extern u32 Read_Addr;
extern u32 Go_Addr;

extern u32 TimeoutCount;

extern u32 APPCode[512];
extern u32 ChunkSumXor;  /*和校验的异或*/
extern u32 ChunkReadIndex; /*读取索引*/
extern u32 ChunkSum; /*求和校验*/
extern u16 CurrentSeq;
extern u32 DecryptKey;
/* Exported functions ------------------------------------------------------------*/
void CAN_BOOT_JumpToApplication(u32 Addr);
void ProcessCanCMD(void);
void SendCanResp(CMDTYPE Cmd, u8 Status);
void BTLED_GPIO_Config(void);
int Bootload_Func(char *CmdStr, char *CmdBuf);
void Bootload_Init(int deviceId);
int BootLoad_ServerInIrq(CanRxMsg* RxMsg, int deviceId);
/* Used by aia_bootload.c---------------------------------------------------------*/
extern u8 ModuleAdress;
extern char ModuleAdressChar;
u16 atoi16(char *p, char radix);



#endif







