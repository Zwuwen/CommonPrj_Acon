/**
  ******************************************************************************
  * @file    AIA_PID.h
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
  * @brief   AIA_Library 
  ******************************************************************************
  * @attention
  * Proportional-Integral-Derivative (PID) control
  * Implements a PID controller using a PID algorithm for simple PID applications
  * or high speed control applications that require an efficient algorithm. The PID 
  * algorithm features control output range limiting with integrator anti-windup 
  * and bumpless controller output for PID gain changes.
  *
  * A typical PID lib contains 2 files. There are one core file: AIA_PID.c,
  * one configuration file: AIA_PID.h
  * refer to : NI_PID_pid.lvlib:PID.vi
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
