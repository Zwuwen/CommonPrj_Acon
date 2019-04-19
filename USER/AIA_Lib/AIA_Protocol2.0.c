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
#include <stdarg.h>
#include "AIA_Protocol2.0.h"
#include "AIA_Utilities.h" 
#include "CAN_Driver.h"
#include "AIA_CmdFIFO.h" 
#include "AIA_Bootload.h"
#include "AIA_ModuleCore.h"
#include "AIA_ErrorCode.h"
#include "stdio.h"

/* Private function prototypes -----------------------------------------------*/
void ReceiveCanFrame_InIrq(AIAMODULE *module, CanRxMsg *rxMsg, int bcflag);
int ProcessNewCmd(AIAMODULE *module);
int ParseCmdParam(char *cmd, int *val, int num);



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
		ReceiveCanFrame_InIrq(&ModuleCore, rxMsg, 0);
		return;
	}
	
	if(rxMsg->StdId == ModuleCore.boardcastRecvSignature)
	{
		ReceiveCanFrame_InIrq(&ModuleCore, rxMsg, 1);
		/*boardcast frame don't need return.*/
	}
	
	
	
}



/**
  * @brief  
  * @param  
  * @retval None
  */
void ReceiveCanFrame_InIrq(AIAMODULE *module, CanRxMsg *rxMsg, int bcflag)
{
	int i;
	
	for(i=0; i<rxMsg->DLC; i++)
	{
		if(rxMsg->Data[i] == '&')
		{
			module->fifo.flag.Bit.hasFrameHead = 1;
			module->fifo.pRecvBuf[1] = bcflag;	 
			module->fifo.pRecvBuf[2] = '&';	   /*[0] is length; [1] broadcast flag*/
			module->fifo.currRecvLength = 3;	
			module->fifo.flag.Bit.receiveCompleted = 0;	
		}
		else if(rxMsg->Data[i] == '\r')
		{
			if(module->fifo.flag.Bit.hasFrameHead == 1)
			{
				module->fifo.flag.Bit.hasFrameHead = 0;
				if(module->fifo.currRecvLength < 7)
					continue;
				
				module->fifo.pRecvBuf[module->fifo.currRecvLength] = '\r';
				module->fifo.pRecvBuf[module->fifo.currRecvLength+1] = '\0';
				module->fifo.pRecvBuf[0] = module->fifo.currRecvLength - 2;
				
				if((module->fifo.pRecvBuf[5] == 'Z') && (module->fifo.pRecvBuf[6] == 'Z'))
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
			if(module->fifo.flag.Bit.hasFrameHead == 0)
				continue;
			
			if(module->fifo.currRecvLength < COMMANDLENGTH)
				module->fifo.pRecvBuf[module->fifo.currRecvLength++] = rxMsg->Data[i];
			else 
				module->fifo.flag.Bit.hasFrameHead = 0;
		}
	}
}



/**
  * @brief  
  * @param  
  * @retval None
  */
void SendModuleResponse(AIAMODULE *module)
{
	sendNByteDataViaCan(&module->responseBuf[1], module->responseBuf[0], module->address, CAN_ID_STD);
	
	if(module->recvFrame->sequence == '0')
		return;
	
	module->sequence = (module->sequence == '9') ? '1' : (module->sequence + 1);
}




/**
  * @brief  
  * @param  
  * @retval None
  */
void AIA_Protocol2_Handle(AIAMODULE *module)
{
	int ret;
	
	if(module->fifo.cmdNumber == 0)
		return;
	
	if(GetCmdFromFIFO(&(module->fifo)) == FALSE)
		return;
	
	module->recvFrame = (FRAMEFORMAT*)module->fifo.pOutBuf;
	
	ret = ProcessNewCmd(module);
	
	if(ret == RESPONSE_IN_PROCESS)
		return;
	
	if(ret == PREPARE_IN_PROCESS)
	{
		//Nothing to do;
	}
	else
	{
		PrepareResponseBuf(module, "%d", ret);
	}
	
	SendModuleResponse(module);
}




/**
  * @brief  
  * @param  
  * @retval None
  */
void PrepareResponseBuf(AIAMODULE *module, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	module->responseBuf[1] = '&';
	module->responseBuf[2] = module->addressChar;
	module->responseBuf[3] = module->recvFrame->sequence;
	module->responseBuf[4] = module->recvFrame->cmdHigh  + ' '; /*lower case*/
	module->responseBuf[5] = module->recvFrame->cmdLow  + ' ';  /*lower case*/
	
	module->responseBuf[0] = 6;
	module->responseBuf[0] += vsprintf(&(module->responseBuf[module->responseBuf[0]]), fmt, args);
	module->responseBuf[module->responseBuf[0]] = '\r';
	module->responseBuf[module->responseBuf[0]+1] = '\0';
	
	va_end(args);
}



/**
  * @brief  
  * @param  
  * @retval None
  */
int ProcessNewCmd(AIAMODULE *module)
{
	int ret;
	int cmdWord;
	FRAMEFORMAT *pFrame;
	char expectIdChar;
	
	ret = FAIL;
	
	pFrame = module->recvFrame;
	expectIdChar = (pFrame->bcflag == 0) ? module->addressChar : module->boardcastIdChar;
	
	if(pFrame->devIdChar != expectIdChar)	/* check address*/
		return ERR_ADDRESS;
	
	if((pFrame->sequence != '0') && 		/*'0' means needn't match seq*/
	   (pFrame->sequence != module->sequence))   /*check sequence*/
	{
		return ERR_SEQUENCE;
	}
	
	cmdWord = UPCASE2INT(pFrame->cmdHigh, pFrame->cmdLow);
	module->validParams = ParseCmdParam(pFrame->buf, module->recvParams, MAXIMUM_SUPPORT_RECV_PARAMS);
	
	if(pFrame->bcflag == 1) /*board cast cmd only support the RA*/
	{
		if(module->BoardCastProcess == NULL)
			ret = ERR_CMDNOTIMPLEMENT;
		else
			ret = module->BoardCastProcess(module, cmdWord);
		
		return ret;
	}
	
	if(module->NormalProcess == NULL)
		ret = ERR_CMDNOTIMPLEMENT;
	else
		ret = module->NormalProcess(module, cmdWord);

	return ret;
	
	
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
  * @brief  Get the Param Length, the ParamString could be anythin except reserved characters.
  *			',': separator between two params.
  *			'\r': indicate the end of cmd frame.
  * @param  pCmd: Param after XX cmd..
  * @retval The ParamString Length.
  */
static int GetParamLength(char *pCmd)
{
	int i=0;
	while((*pCmd != ',')&&(*pCmd != ' ')&&(*pCmd != '\r'))
	{
		pCmd++;
		i++;		
	}
	return i;
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
	len = 0;
	p = cmd + len;
	if(*p == '\r')
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














