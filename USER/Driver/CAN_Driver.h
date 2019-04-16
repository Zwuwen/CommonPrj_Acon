/**
  ******************************************************************************
  * @file    CAN_Driver.h
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
#ifndef __CAN_DRIVER_H
#define	__CAN_DRIVER_H
#include "stm32f10x.h"

#define FILTER_FRAMEID_NUMBER 2


#define GPIO_CAN                   GPIOA
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOA
#define GPIO_Pin_CAN_RX            GPIO_Pin_11
#define GPIO_Pin_CAN_TX            GPIO_Pin_12



/* Exported variables ------------------------------------------------------- */
extern int CanFilterSignature[FILTER_FRAMEID_NUMBER];


/* Exported functions ------------------------------------------------------- */
void CAN_Driver_Init(void);
void sendNByteDataViaCan(char *pData, int len, uint32_t frameID, uint32_t IdType);
void sendStringViaCan(char *pStr, uint32_t frameID, uint32_t IdType);
void CAN_Filter_Config(int *frameSignature, int len);

#define CANSendStringExt(pSTRING, FRAMEID) sendStringViaCan(pSTRING, FRAMEID, CAN_ID_EXT)
#define CANSendStringStd(pSTRING, FRAMEID) sendStringViaCan(pSTRING, FRAMEID, CAN_ID_STD)
#define CANSendString(pSTRING, FRAMEID) CANSendStringStd(pSTRING, FRAMEID)


#endif
