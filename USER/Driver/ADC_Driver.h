/**
  ******************************************************************************
  * @file    ADC_Driver.h
  * @author  Bangju.Jin
  * @version V1.0
  * @created    19-April-2019
  * @lastModified 19-April-2019
  * @brief   ADC Driver
  ******************************************************************************
  * @attention
  * See TODO
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __ADC_DRIVER_H
#define	__ADC_DRIVER_H

#include "AIA_Utilities.h"



//TODO
#define Sample_Num 20//取平均值的数量
#define Channel_Num 2//通道数量

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
//TODO
#define ADC_IN_PIN1		GPIO_Pin_6
#define ADC_IN_PIN2		GPIO_Pin_7
#define ADC_IN_GPIO	GPIOA

extern u16 ADC_Value[Channel_Num];
/* Exported functions ------------------------------------------------------- */
void ADC_Driver_Init(void);
void Updata_Adc_Value(void);
#endif
