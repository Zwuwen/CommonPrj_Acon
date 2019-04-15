/**
  ******************************************************************************
  * @file    AIA_Protocol2.0.c 
  * @author  Bowen.He
  * @version V2.0
  * @created    15-April-2019
  * @lastModify 
  * @brief   AIA_Protocol 2.0:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************
*/ 
#include "AIA_Protocol2.0.h"
#include "AIA_Utilities.h" 
#include "CAN_Driver.h"
#include "AIA_CmdFIFO.h" 
#include "AIA_Bootload.h"
#include "AIA_ModuleCore.h"

/* Private function prototypes -----------------------------------------------*/
void ReceiveCanFrame_InIrq(AIAMODULE *module, CanRxMsg *rxMsg);
void ProcessNewCmd(AIAMODULE *module);


/**
  * @brief  
  * @param  
  * @retval None
  */
void DistributeNewCanFrame_InIrq(CanRxMsg *rxMsg)
{
	
	if(BootLoad_ServerInIrq(rxMsg, ModuleCore.address) == 1) 
		return;
	
	if(rxMsg->IDE != CAN_ID_STD)
		return;
	
	if(rxMsg->StdId == ModuleCore.normalRecvSignature)
	{
		ReceiveCanFrame_InIrq(&ModuleCore, rxMsg);
		return;
	}
	
	if(rxMsg->StdId == ModuleCore.boardcastRecvSignature)
	{
		
		/*boardcast frame don't need return.*/
	}
	
	
	
}




void ReceiveCanFrame_InIrq(AIAMODULE *module, CanRxMsg *rxMsg)
{
	int i;
	
	for(i=0; i<rxMsg->DLC; i++)
	{
		if(rxMsg->Data[i] == '&')
		{
			module->fifo.flag.Bit.hasFrameHead = 1;
			module->fifo.pRecvBuf[1] = '&';	    /*avoid copy frame*/
			module->fifo.currRecvLength = 2;	/*Cmd[0] is length*/
			module->fifo.flag.Bit.receiveCompleted = 0;	
		}
		else if(rxMsg->Data[i] == '\r')
		{
			if(module->fifo.flag.Bit.hasFrameHead == 1)
			{
				module->fifo.flag.Bit.hasFrameHead = 0;
				module->fifo.pRecvBuf[module->fifo.currRecvLength] = '\r';
				module->fifo.pRecvBuf[module->fifo.currRecvLength+1] = '\0';
				module->fifo.pRecvBuf[0] = module->fifo.currRecvLength - 1;
				
				if((module->fifo.pRecvBuf[3] == 'Z') && (module->fifo.pRecvBuf[4] == 'Z'))
				{
					ClearCmdFIFO(&(module->fifo));
					module->flag.Bit.terminate = 1;
				}
				
				PutCmdToFIFO(&(module->fifo));
				module->fifo.flag.Bit.receiveCompleted = 1;
			}
		}
		else
		{
			if(module->fifo.currRecvLength < COMMANDLENGTH)
				module->fifo.pRecvBuf[module->fifo.currRecvLength++] = rxMsg->Data[i];
			else 
				module->fifo.flag.Bit.hasFrameHead = 0;
		}
	}
}



void AIA_Protocol2_Handle(AIAMODULE *module)
{
	if(module->fifo.cmdNumber == 0)
		return;
	
	if(GetCmdFromFIFO(&(module->fifo)) == FALSE)
		return;
	
	ProcessNewCmd(module);
	
}


void ProcessNewCmd(AIAMODULE *module)
{
	
}

  
 
//void ResponseCmdByCan(int ret)
//{
//	char AnswerStr[20];
//	if(ret != RESPONSE_IN_PROCESS)
//	{
//		if(ret == PASS )
//		{
//			sprintf(AnswerStr,"&%cOK\r", ModuleAdressChar);
//			CANSendString(AnswerStr, ModuleAdress);		
//		}
//		else
//		{
//			sprintf(AnswerStr,"&%cERR %d\r", ModuleAdressChar, ret);
//			CANSendString(AnswerStr, ModuleAdress);	
//		}
//	}	
//}





/**
  * @brief  Get the Param Length, the ParamString could be anythin except reserved characters.
  *			',': separator between two params.
  *			'\r': indicate the end of cmd frame.
  * @param  pCmd: Param after XX cmd..
  * @retval The ParamString Length.
  */
static int GetParamLength(char *pCmd)
{
	int i=0;
	while((*pCmd != ' ')&&(*pCmd != '\r'))
	{
		pCmd++;
		i++;		
	}
	return i;
}


/**
  * @brief  Convert the ASCII to int, String ending with 'H' means Hexadecimal, 
  *			and Hexadecimal is unsigned. Notice INT overflow, while the string is too long.
  * @param  Str: The ASCII like: -100, 23, F1H: Upper Case
  * @param  Result: The Result. It will be zero, if there is an error in converting.
  * @param  length: the String Length, include the '-' and 'H'; 
  * @retval Enum indicates conversion succeed which will be 
  *         RET_SUCCESS or RET_FAIL.
  */
RETSTAT StrToDec(char *Str, int *Result, int length)
{
	int i,ret,SignBit,radix;
	ret = 0;
	SignBit = 1;
	radix = 10; /*Decimal*/
	i = 0;
	
	if((Str[i] == '+') || (Str[i] == '-'))
	{
		if(Str[i] == '-') SignBit = -1;
		i++;
	}
	else i = 0;
	
	if(length <= i) return RET_FAIL; 		 /*No Data Space*/
	if(Str[length-1] == 'H') 		 		 /*Hex*/
	{
		if(SignBit == -1) return RET_FAIL;  /*Hex has no signbit*/
		length--;
		radix  = 16;   				 		/*Hexadecimal*/
		if(length > 8) return RET_FAIL;		/*int 0xAABBCCDD  max int*/		
	}
	for(; i<length ; i++)
	{
		if(radix == 10)
		{
			if((Str[i] >= '0') && (Str[i] <= '9'))
				ret = ret*radix + (Str[i] - '0');
			else return RET_FAIL;  		   /*unexcept character*/	
		}	
		else if(radix == 16)
		{
			if((Str[i] >= '0') && (Str[i] <= '9'))
				ret = ret*radix + (Str[i] - '0');
			else if((Str[i] >= 'A') && (Str[i] <= 'F'))
				ret = ret*radix + (Str[i] - 'A' + 10);
			else return RET_FAIL; 		  /*unexcept character*/		
		}
	}
	*Result = ret * SignBit;
	return RET_SUCCESS;
}


/**
  * @brief  
  * @param  str:
  * @param  val:
  * @param  num:  
  * @retval The Valid num.
  */
int ParseCmdParam(char *cmd, int *val, int num)
{
	int len, i;
	char *p;
	len = GetParamLength(cmd);
	p = cmd + len;
	if(*p++ == '\r')
	{
		return 0;
	}
	i = 0;
	while(i< num)
	{
		len = GetParamLength(p);
		if(StrToDec(p, &val[i], len) == RET_FAIL) return i;
		p += len;
		if(*p++ == '\r') return i+1;
		i++;
	}
	return num;
}














