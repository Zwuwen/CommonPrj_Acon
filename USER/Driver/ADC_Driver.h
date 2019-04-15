/**
  ******************************************************************************
  * @file    ADC_Driver.c 
  * @author  AIA Team
  * @version V1.1
  * @created    15-April-2019
  * @lastModify 
  * @brief   ADC Driver:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __ADC_DRIVER_H
#define	__ADC_DRIVER_H

#include "AIA_Utilities.h"


#define ADC_BUFSIZE 32*2
extern int CH_Value[2];


#define PT100_PIN1		GPIO_Pin_6
#define PT100_PIN2		GPIO_Pin_7
#define PT100_GPIO	GPIOA



#define DAC_OUT1_PIN		GPIO_Pin_4
#define DAC_OUT2_PIN		GPIO_Pin_5
#define DAC_OUT_GPIO		GPIOA
/* Exported variables ------------------------------------------------------- */
extern int DacValue1, DacValue2;
extern int LastDacValue1, LastDacValue2;


/* Exported functions ------------------------------------------------------- */
void ADC1_Init(void);
void Update_ADC1_CH_Value(__IO int *value);
void DAC_Configuration(void);

#define DAC_SET_CHN1(DACVALUE) DAC_SetChannel1Data(DAC_Align_12b_R, DACVALUE)//;\
							  DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE)
#define DAC_SET_CHN2(DACVALUE) DAC_SetChannel2Data(DAC_Align_12b_R, DACVALUE)//;\
							  DAC_SoftwareTriggerCmd(DAC_Channel_2,ENABLE)



#endif
