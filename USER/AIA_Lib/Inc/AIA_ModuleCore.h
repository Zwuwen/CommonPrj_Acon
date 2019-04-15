/**
  ******************************************************************************
  * @file    AIA_ModuleCore.h
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
  * @brief   AIA_Library 
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
#ifndef __AIA_MODULECORE_H
#define	__AIA_MODULECORE_H

#include "stm32f10x.h"
#include "AIA_CmdFIFO.h"



/* Exported macro ------------------------------------------------------------*/
#define FIRST_VER		(1)	
#define MIDDLE_VER		(0)
#define TEMP_VER		(0)

#define MODULE_NAME "AIA_Common_Module"

/* Exported Function Macro ------------------------------------------------------------*/
extern const char IdChar[];






/* Exported typedef ------------------------------------------------------------*/
typedef	struct
{
	int address;	/**/
	
	char addressChar;
	char dummys[3];
	
	int normalRecvSignature;
	int boardcastRecvSignature;
	
	char Name[20];
	char responseBuf[100];
	
	CMDFIFO fifo;
	
	/*Flags*/
	union{ 
		struct{
			unsigned init 			: 1;  	/*initialize.*/
			unsigned terminate		: 1;    /*terminate sign.*/
		}Bit;
		int Allbits;
	}flag;
}AIAMODULE;

extern AIAMODULE ModuleCore;



extern uint8_t	const HEX_2_ASCII[];

/* Exported functions ------------------------------------------------------- */
void ModuleCore_Init(void);




#endif
