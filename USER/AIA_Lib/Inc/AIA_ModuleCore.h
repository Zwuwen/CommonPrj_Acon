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

#define RECEIVE_PARAMS_NUMBER_MAX 10

/* Exported Function Macro ------------------------------------------------------------*/
extern const char IdChar[];






/* Exported typedef ------------------------------------------------------------*/
typedef struct _FRAMEFORMAT
{
	char len;
	char bcflag; /*broadcast cmd*/
	char frameHead;
	char devIdChar;
	char sequence;
	char cmdHigh;
	char cmdLow;
	char buf[COMMANDLENGTH-7];
}FRAMEFORMAT;


typedef	struct
{
	int address;	/**/
	char addressChar;
	char boardcastIdChar;
	char sequence; /* '1'~ '9' : '0' means needn't to match seq.*/
	char dummys[2];
	
	int normalRecvSignature;
	int boardcastRecvSignature;
	
	int validParams;
	int recvParams[RECEIVE_PARAMS_NUMBER_MAX];
	
	int responseLen;
	char Name[20];
	char responseBuf[100];
	
	FRAMEFORMAT *recvFrame;
	
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
