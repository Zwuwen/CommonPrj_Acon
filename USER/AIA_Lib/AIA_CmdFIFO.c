/**
  ******************************************************************************
  * @file    AIA_PID.c
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
#include "AIA_CmdFIFO.h" 
#include "string.h"


 /**
  * @brief  初始化命令队列.
  * @param  None
  * @retval None
  */
void InitCmdFIFO(CMDFIFO *cmdFifo)
{
//	memset(cmdFifo,0,sizeof(CMDFIFO));
	
	cmdFifo->addrIn = 0;
	cmdFifo->addrOut = 0;
	cmdFifo->cmdNumber = 0;
	cmdFifo->pOutBuf = NULL;
	cmdFifo->currRecvLength = 0;
	cmdFifo->pRecvBuf = &cmdFifo->Cmd[cmdFifo->addrIn][0];
	cmdFifo->flag.Bit.isInit = 1;	
} 




void PutCmdToFIFO(CMDFIFO *cmdFifo)
{
	if(cmdFifo->flag.Bit.isInit != 1)
		return;
		
	if(cmdFifo->cmdNumber < FIFOLENGTH-1)
	{
		cmdFifo->cmdNumber++;
		cmdFifo->addrIn = (cmdFifo->addrIn == FIFOLENGTH-1) ? 0 : cmdFifo->addrIn+1;
		cmdFifo->pRecvBuf = &cmdFifo->Cmd[cmdFifo->addrIn][0];
	}
}  



BOOL GetCmdFromFIFO(CMDFIFO *cmdFifo)
{
	if(cmdFifo->flag.Bit.isInit != 1)
		return FALSE;
	
	__disable_irq();
	if(cmdFifo->cmdNumber > 0)
	{
		cmdFifo->cmdNumber--;
		cmdFifo->pOutBuf = cmdFifo->Cmd[cmdFifo->addrOut];
		cmdFifo->addrOut = (cmdFifo->addrOut== FIFOLENGTH-1) ? 0 : cmdFifo->addrOut+1;
		__enable_irq();
		return TRUE;
	}
	cmdFifo->pOutBuf = NULL;
	__enable_irq();
	return FALSE;
}




void ClearCmdFIFO(CMDFIFO *cmdFifo)
{
	if(cmdFifo->flag.Bit.isInit != 1)
		return;
	
	__disable_irq(); 	
	InitCmdFIFO(cmdFifo);
	__enable_irq();
	
}  
  
  
  
  
  
  
  






































