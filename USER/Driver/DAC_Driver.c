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
#include "DAC_Driver.h"

/**
  * @brief  初始化DAC引脚
  * @param  None
  * @retval None
  */
static void DAC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	//打开GPIOA时钟和DAC时钟
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
  * @brief  配置DAC，最后启动DAC 默认值0
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
  * @brief  初始化DA转换模块DAC
  * @param  None
  * @retval None
  */
void DAC_Driver_Init(void)
{
	DAC_GPIO_Config();
	DAC_Mode_Config();
	
}
