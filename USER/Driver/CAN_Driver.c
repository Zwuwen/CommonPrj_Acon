/**
  ******************************************************************************
  * @file    CAN_Driver.c 
  * @author  Bowen.He
  * @version V1.0
  * @created    15-April-2019
  * @lastModified 15-April-2019
  * @brief   CAN Driver
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#include "CAN_Driver.h"
#include "string.h"


int CanFilterSignature[FILTER_FRAMEID_NUMBER];


/**
  * @brief  Configure the CAN peripheral.
  * @param  moduleId: CoreModule Device id.
  * @retval None
  */
static void CAN_Configuration()
{
    GPIO_InitTypeDef  	   GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;

	////////////////CAN RCC Init//////////////////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIO_CAN, ENABLE);
	
	////////////////CAN GPIO Init//////////////////
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_TX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_CAN, &GPIO_InitStructure);	  
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CAN_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_CAN, &GPIO_InitStructure);

	
	////////////////CAN CAN_InitStructure//////////
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_TTCM = DISABLE;	 
    CAN_InitStructure.CAN_ABOM = DISABLE;	 
    CAN_InitStructure.CAN_AWUM = DISABLE;	
    CAN_InitStructure.CAN_NART = DISABLE;	 
    CAN_InitStructure.CAN_RFLM = DISABLE;	 
    CAN_InitStructure.CAN_TXFP = DISABLE;	
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;   
    CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;	
    CAN_InitStructure.CAN_BS1 = CAN_BS1_16tq;	  
    CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
    CAN_InitStructure.CAN_Prescaler = 18;		//100KB Baudrate
    CAN_Init(CAN1,&CAN_InitStructure);
}


/**
  * @brief  
  * @param  frameSignature: Bit31: IsRemoteFrame.    Bit30: IsExternFrame
  * @retval None
  */
void CAN_Filter_Config(int *frameSignature, int len)
{
	int total, remain;
	int FrameType1, FrameType2,i,FrameExtFlag;
	
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	total = (len > 28)? 14: len/2;		
	remain = (len > 28)? 0: len%2;
		
	for(i=0; i<total; i++)
	{
		CAN_FilterInitStructure.CAN_FilterNumber 		= 	i;
		CAN_FilterInitStructure.CAN_FilterMode 			= 	CAN_FilterMode_IdList;
		CAN_FilterInitStructure.CAN_FilterScale 		= 	CAN_FilterScale_32bit;
			
		FrameExtFlag = frameSignature[2*i] & 0x40000000;
		FrameType1 	= (frameSignature[2*i] & 0x80000000)? CAN_RTR_REMOTE : CAN_RTR_DATA;
		FrameType2	= (frameSignature[2*i] & 0x40000000)? CAN_ID_EXT : CAN_Id_Standard;
		
		CAN_FilterInitStructure.CAN_FilterIdHigh   		= 	(((u32)(frameSignature[2*i]|0X80)<<(FrameExtFlag? 3 : 21))&0xffff0000)>>16;		/*module ID*/
		CAN_FilterInitStructure.CAN_FilterIdLow   		= 	(((u32)(frameSignature[2*i]|0X80)<<(FrameExtFlag? 3 : 21))|(FrameType1)|(FrameType2))&0xffff;

		FrameExtFlag = frameSignature[2*i+1] & 0x40000000;
		FrameType1 	= (frameSignature[2*i+1] & 0x80000000)? CAN_RTR_REMOTE : CAN_RTR_DATA;
		FrameType2	= (frameSignature[2*i+1] & 0x40000000)? CAN_ID_EXT : CAN_Id_Standard;
		
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh   		= (((u32)(frameSignature[2*i+1]|0X80)<<(FrameType2? 3 : 21))&0xffff0000)>>16;		/*module ID*/
		CAN_FilterInitStructure.CAN_FilterMaskIdLow   		= (((u32)(frameSignature[2*i+1]|0X80)<<(FrameType2? 3 : 21))|(FrameType1)|(FrameType2))&0xffff;
		
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 	0;
		CAN_FilterInitStructure.CAN_FilterActivation = 		ENABLE;	 
		CAN_FilterInit(&CAN_FilterInitStructure);
	}	
	if(remain)
	{
		FrameExtFlag = frameSignature[len-1] & 0x40000000;
		FrameType1 	= (frameSignature[len-1] & 0x80000000)? CAN_RTR_REMOTE : CAN_RTR_DATA;
		FrameType2	= (frameSignature[len-1] & 0x40000000)? CAN_ID_EXT : CAN_Id_Standard;
		
		CAN_FilterInitStructure.CAN_FilterNumber 		= 	total + 1;
		CAN_FilterInitStructure.CAN_FilterMode 			= 	CAN_FilterMode_IdMask;
		CAN_FilterInitStructure.CAN_FilterScale 		= 	CAN_FilterScale_32bit;
		
		CAN_FilterInitStructure.CAN_FilterIdHigh   		= 	(((u32)(frameSignature[len-1]|0X80)<<(FrameExtFlag? 3 : 21))&0xffff0000)>>16;		/*module ID*/
		CAN_FilterInitStructure.CAN_FilterIdLow   		= 	(((u32)(frameSignature[len-1]|0X80)<<(FrameExtFlag? 3 : 21))|(FrameType2)|(FrameType1))&0xffff;
		
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh   		= 	0xFFFF;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow   		= 	0xFFFF;
		
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 	0;
		CAN_FilterInitStructure.CAN_FilterActivation = 		ENABLE;	 
		CAN_FilterInit(&CAN_FilterInitStructure);
	}
}


/**
  * @brief  Initialize the CAN peripheral.
  * @param  None.
  * @retval None
  */
void CAN_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure; 
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
}



/**
  * @brief  Initialize the CAN peripheral.
  * @param  None.
  * @retval None
  */
void CAN_Driver_Init(void)
{
    CAN_Configuration();
	
	CAN_NVIC_Config();
	
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	
}





/**
  * @brief  Send N Bytes data via can. 
  *          If len == 0, then send can frame only with frameID (DLC=0).
  * @param  *pData:  allow '\0' within the stream.
  * @param  len : Send Data length.
  * @param  frameId: Can Frame Id. Standard Frame: 11bit; Extern Frame 29bit;
  * @param  IdType: only can be: CAN_ID_STD or CAN_ID_EXT
  * @retval None
  */
void sendNByteDataViaCan(char *pData, int len, uint32_t frameID, uint32_t IdType)
{
	CanTxMsg txMsg;
	int strLength, remainNumber;
	u8 transmitMailbox;
	
	transmitMailbox = 0;
	strLength = len;
	remainNumber = strLength;

	txMsg.StdId = frameID; 	
	txMsg.ExtId = 0x01;			
	txMsg.RTR 	= CAN_RTR_DATA;	
	txMsg.IDE 	= IdType;

	do{
		if(remainNumber > 8)
		{
			txMsg.DLC = 8;
			memcpy(txMsg.Data, &pData[strLength - remainNumber], txMsg.DLC);
			remainNumber -= 8; 
		}
		else
		{
			txMsg.DLC = remainNumber;
			memcpy(txMsg.Data, &pData[strLength - remainNumber], txMsg.DLC);
			remainNumber = 0; 
		}
		
		transmitMailbox = CAN_Transmit(CAN1,&txMsg);	 		
		while(CAN_TransmitStatus(CAN1, transmitMailbox) != CANTXOK); //wait can frame send ok.
	}while(remainNumber != 0);
}




/**
  * @brief  Send String via can. If *pStr == NULL, then send empty frame(DLC=0).
  * @param  *pStr: String Point.
  * @param  frameId: Can Frame Id. Standard Frame: 11bit; Extern Frame 29bit;
  * @param  IdType: only can be: CAN_ID_STD or CAN_ID_EXT
  * @retval None
  */
void sendStringViaCan(char *pStr, uint32_t frameID, uint32_t IdType)
{
	int len;
	len = strlen(pStr);
	sendNByteDataViaCan(pStr, len, frameID, IdType);
}



