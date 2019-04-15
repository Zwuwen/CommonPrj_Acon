/**
  ******************************************************************************
  * @file    AIA_SyncData.c
  * @author  Bowen.he
  * @version V2.0
  * @date    27-March-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  * This lib is used for real-time data transmit. 
  * CAN standard frame id has 11bit. AIA device use the low 8bit, high 3bit is useless
  * for the AIA_Procotol.
  * Use (DeviceID | 0x700) as can frameID, for hexadecimal data transmission.
  * can data area have 8 byte: first byte is total package number. second byte is package index.
  * The remaining six bytes are effective data. 
  * For example: package 1 :0x02 0x01 0x01 0x02 0x03 0x04 0x05 0x06
  * 			 package 2 :0x02 0x02 0x07 0x08 0x09 0x0a 0x0b 0x0c
  * transmit the valid data: 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
  * A typical SyncData lib contains 2 files. There are one core file: AIA_SyncData.c,
  * one configuration file: AIA_SyncData.h
  * Use with AIASyncData.exe
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
#include "AIA_SyncData.h"
#include "CAN_Driver.h"
#include "AIA_ErrorCode.h"
#include "stdio.h"
#include "string.h"

_SYNCDATA SyncData;

char SyncBuffer[100];

/**
  * @brief  SyncData Initialize
  * @param  *val
  * @retval None
  */  
void SyncData_Init(void)
{
	char tmpbuf[20];
	char *p;
	SyncData.uploadPeriod = DEFAULT_SYNCDATA_UPLOAD_PERIOD;
	SyncData.periodCount_Total = SyncData.uploadPeriod / UPDATE_SYNCDATAFLAG_IRQ_MS;
	SyncData.periodCount_i = 0;
	SyncData.flag.Bit.init = 1;
	SyncData.flag.Bit.syncTask = 0;
	strcpy(tmpbuf, SYNCDATA_FORMAT);
	p = tmpbuf;
	SyncData.validByteNumber = 0;
	while(*p != NULL)
	{
		SyncData.validByteNumber += *p++ -'0';
	}
	
}



/**
  * @brief  Upload SyncData
  * @param  *val
  * @retval None
  */  
void aia_syncdata_upload(char *val, int len, int id)
{
	int totalFrame;
	int frameIndex;
	int i, n;
	int valIndex;
	CanTxMsg txMsg;
	u8 TransmitMailbox;
	TransmitMailbox = 0;
	
	if(len <1)
		return;
	
	valIndex = 0;
	totalFrame = (len + 5) / 6;
	frameIndex = 1;
	
	for(i=1; i <= totalFrame; i++)
	{
		txMsg.StdId = id | 0x700;
		txMsg.ExtId = 0x01;
		txMsg.RTR 	= CAN_RTR_DATA;
		txMsg.IDE 	= CAN_ID_STD;
		txMsg.Data[0] = totalFrame;
		txMsg.Data[1] = frameIndex++;
		
		for(n=2; n<8; n++)
		{
			txMsg.Data[n] = val[valIndex++];
			
			if(valIndex >= len)
			{
				n++;
				break;
			}
		}
		txMsg.DLC 	= n;		
		TransmitMailbox = CAN_Transmit(CAN1, &txMsg);	 		
		while(CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK);
	}
}	
  

/**
  * @brief  Upload SyncData
  * @param  id: device id
  * @retval None
  */ 
void AIA_SyncData_Upload(int id)
{
	if(!SyncData.flag.Bit.syncTask) return;
	if(SyncData.flag.Bit.uploadOnce)
	{
		SyncData.flag.Bit.uploadOnce = 0;
		aia_syncdata_upload(SyncBuffer, SyncData.validByteNumber, id);
	}
	
}

/**
  * @brief  Called in Timer IRQ. Update the Count_dt. 
  *         change flag :uploadOnce
  * @param  None
  * @retval None
  */  
void SyncData_UpdatePeriod_InIrq(void)
{
	if(!SyncData.flag.Bit.syncTask)
		return;
	
	SyncData.periodCount_i++;
	if(SyncData.periodCount_i > SyncData.periodCount_Total)
	{
		SyncData.periodCount_Total = 0;
		SyncData.flag.Bit.uploadOnce = 1;
	}
}




int ParseCmdParam(char *cmd, int *val, int num);
void ResponseCmdByCan(int ret);
/**
  * @brief  
  * @param  *CmdStr: Command Word
  * @param  *Cmd: Receive Buf.
  * @retval ret
  */
int SYNC_Command_Func(char *CmdStr, char *Cmd)
{
	int isSynccmd;
	char AnswerStr[50];
	int ret;
	int firstalign;
	ret = ERR_PARAM;
	isSynccmd = 0;
	
	firstalign = (Cmd[1] == '&') ? 1 : 0;
	
	if(strcmp(CmdStr, "SYNCA") == 0)
	{
		/*LEN &1SYNCA*/
		int val[1];
		ret = ERR_PARAM;
		if(ParseCmdParam(Cmd + firstalign, val, 1) == 1)
		{
			SyncData.uploadPeriod = val[0];				
		}
		else
		{
			SyncData.uploadPeriod = DEFAULT_SYNCDATA_UPLOAD_PERIOD;
		}
		SyncData.periodCount_Total = SyncData.uploadPeriod / UPDATE_SYNCDATAFLAG_IRQ_MS;
		SyncData.flag.Bit.syncTask = 1;
		isSynccmd = 1;
		ret = PASS;	
		
//		sprintf(AnswerStr,"&%cOK %s\r", ModuleAdressChar, SYNCDATA_FORMAT);
//		
//		CANSendString(AnswerStr, ModuleAdress);	
		return 1;
	}
	else if(strcmp(CmdStr,"SYNCZ") == 0)
	{
		SyncData.flag.Bit.syncTask = 0;
		ret = PASS;
		isSynccmd = 1;
	}

	if(isSynccmd == 1)
	{
//		ResponseCmdByCan(ret);
		return 1;
	}
	return  0;
}


























