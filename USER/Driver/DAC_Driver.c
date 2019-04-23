/*
  * @file    DAC_Driver.c 
  * @author  AIA Team
  * @version V1.1
  * @created    18-April-2019
  * @lastModify 
  * @brief   DAC Driver:  
*/


//����DAC_Driver.h��ѡ��DACͨ�������Ͳ���ֵ
#include "DAC_Driver.h"

/**
  * @brief  ��ʼ��DAC����
  * @param  None
  * @retval None
  */
static void DAC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	//��GPIOAʱ�Ӻ�DACʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
#if DAC_CHN1_ENABLE
	GPIO_InitStructure.GPIO_Pin = DAC_OUT_PIN1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DAC_OUT_GPIO, &GPIO_InitStructure);	
#endif 
#if DAC_CHN2_ENABLE
	GPIO_InitStructure.GPIO_Pin = DAC_OUT_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DAC_OUT_GPIO, &GPIO_InitStructure);	
#endif 
}

/**
  * @brief  ����DAC���������DAC Ĭ��ֵ0
  * @param  None
  * @retval None
  */
static void DAC_Mode_Config(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	DAC_DeInit();
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	#if DAC_CHN1_ENABLE
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	#endif 
	#if DAC_CHN2_ENABLE
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
	#endif 
}

/**
  * @brief  ��ʼ��DAת��ģ��DAC
  * @param  None
  * @retval None
  */
void DAC_Driver_Init(void)
{
	DAC_GPIO_Config();
	DAC_Mode_Config();
	
}
