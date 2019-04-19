/*
  * @file    ADC_Driver.c 
  * @author  AIA Team
  * @version V1.1
  * @created    18-April-2019
  * @lastModify 
  * @brief   ADC Driver:  
*/
//请在ADC_Driver.h中选择ADC通道数量和布尔值
#include "ADC_Driver.h"


__IO uint16_t ADC_ConvertedValue[Sample_Num][Channel_Num];
u16 ADC_Value[Channel_Num];

/**
  * @brief  初始化ADC引脚
  * @param  None
  * @retval None
  */
static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//TODO
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		
}

/**
  * @brief  配置ADC模块DMA模块
  * @param  None
  * @retval None
  */
static void ADC_Mode_Config(void)
{
	
	
	DMA_InitTypeDef DMA_InitStructure;  
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE ); 
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr =ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = Sample_Num*Channel_Num;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE ); 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADC的采样时钟最快14MHz  
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel =Channel_Num;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	
	//TODO
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6,1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7,2, ADC_SampleTime_239Cycles5);
	
	
	
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



/**
  * @brief  初始化AD采样模块ADC
  * @param  None
  * @retval None
  */
void ADC_Driver_Init(void)
{
	ADC_GPIO_Config();
	ADC_Mode_Config();
}


/**
  * @brief  
  * @param  None
  * @retval ADC1_CH2_MainValue ADC采样的值
  */
void Updata_Adc_Value(void)
{
	int i=0,j=0,sum=0;
	for(;i<Channel_Num;i++)
	{
		sum=0;
		for(j=0;j<Sample_Num;j++)
		{
			sum += ADC_ConvertedValue[j][i];
		}
		ADC_Value[i]=sum/Sample_Num;
	}
}

