/**
  ******************************************************************************
  * @file    AIA_CmdFIFO.h
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
#ifndef __AIA_CMDFIFO_H
#define	__AIA_CMDFIFO_H

#include "AIA_Utilities.h" 

/* Exported typedef ------------------------------------------------------------*/
#define COMMANDLENGTH 60
#define FIFOLENGTH	5
								
								
typedef struct _CMDFIFO
{
	char Cmd[FIFOLENGTH][COMMANDLENGTH];
	char *pRecvBuf;
	char *pOutBuf;
	int addrIn;
	int addrOut;
	int cmdNumber;
	int currRecvLength;
	
	/*Flags*/
	union{ 
		struct{
			unsigned isInit 			: 1;  	/*initialize.*/
			unsigned receiveCompleted 	: 1;	/**/
			unsigned hasFrameHead 		: 1;	/**/
		}Bit;
		int Allbits;
	}flag;
	
}CMDFIFO;



/* Exported functions ------------------------------------------------------- */
void InitCmdFIFO(CMDFIFO *cmdFifo);
void PutCmdToFIFO(CMDFIFO *cmdFifo);
BOOL GetCmdFromFIFO(CMDFIFO *cmdFifo);
void ClearCmdFIFO(CMDFIFO *cmdFifo);


#endif
