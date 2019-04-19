/********************************************************************************
  * @file    ADC_Driver.c 
  * @author  AIA Team
  * @version V1.1
  * @created    19-April-2019
  * @lastModify 
  * @brief   ADC Driver:  
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

/* Exported functions ------------------------------------------------------- */
void ADC_Driver_Init(void);
u16 Read_ADC_AverageValue(int Channel);

#endif
