/**
  ******************************************************************************
  * @file    DAC_Driver.c 
  * @author  Bangju.Jin
  * @version V1.0
  * @created    19-April-2019
  * @lastModified 19-April-2019
  * @brief   DAC Driver
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __DAC_DRIVER_H
#define	__DAC_DRIVER_H

#include "AIA_Utilities.h"


//ѡ��DACͨ�������Ͳ���ֵ ����DAC_Driver_Initʱ����ֹ�ر�����ͨ����
//1=���� 0=�ر�
//����һ��ͨ����API����SET_DAC()
//��������ͨ����API����SET_CHN1_DAC()��SET_CHN2_DAC();
#define DAC_CHN1_ENABLE 1
#define DAC_CHN2_ENABLE 1



#define DAC_OUT_PIN1	GPIO_Pin_4
#define DAC_OUT_PIN2	GPIO_Pin_5
#define DAC_OUT_GPIO	GPIOA

#if DAC_CHN1_ENABLE&&DAC_CHN2_ENABLE
#define SET_CHN1_DAC(DACVALUE) DAC_SetChannel1Data(DAC_Align_12b_R, DACVALUE)
#define SET_CHN2_DAC(DACVALUE) DAC_SetChannel2Data(DAC_Align_12b_R, DACVALUE)
#elif DAC_CHN1_ENABLE
#define SET_DAC(DACVALUE) DAC_SetChannel1Data(DAC_Align_12b_R, DACVALUE)//;
#else
#define SET_DAC(DACVALUE) DAC_SetChannel2Data(DAC_Align_12b_R, DACVALUE)//;
#endif
							  
/* Exported functions ------------------------------------------------------- */
void DAC_Driver_Init(void);
#endif
