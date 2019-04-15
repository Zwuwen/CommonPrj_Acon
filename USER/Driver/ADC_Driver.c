#include "ADC_Driver.h"

uint8_t* u16toa(uint8_t *dest, u16 num);

/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC3_DR_Address    ((uint32_t)0x40013C4C)



/* Private variables ---------------------------------------------------------*/
__IO u16 ADC1_Value[ADC_BUFSIZE];



/* Global variables ---------------Exported in adc.h----------------------------------*/
int CH_Value[2];

int DacValue1, DacValue2;
int LastDacValue1, LastDacValue2;


/**
  * @brief  初始化ADC1 引脚
  * @param  None
  * @retval None
  */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 	 /*72/6 = 12mhz adcclk*/

	/* Enable DMA clock */
  	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

  	/* Configure PA.02 PA.05 PA.06  as analog input */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				// PC0,输入时不用设置速率
}

/**
  * @brief  配置ADC1 DMA1 ，校验ADC1，最后启动ADC1
  * @param  None
  * @retval None
  */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* DMA channel1 configuration */
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_Value[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC_BUFSIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
	 
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channel configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);  //PA4
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_55Cycles5);	 //PA5
	
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



/**
  * @brief  初始化AD采样模块ADC1
  * @param  None
  * @retval None
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}


/**
  * @brief  
  * @param  None
  * @retval ADC1_CH2_MainValue ADC采样的值
  */
void Update_ADC1_CH_Value(__IO int *value)
{
	uint16_t i;

	u32	CH4_MainValue;
	u32	CH5_MainValue;
	
	CH4_MainValue = 0;
	CH5_MainValue = 0;
	
	for(i=0; i<ADC_BUFSIZE; i++)	   /*CH4 CH5*/
	{
		CH4_MainValue += ADC1_Value[i++];
		CH5_MainValue += ADC1_Value[i];
	}
	
	CH4_MainValue /= 32;
	CH5_MainValue /= 32;	
	
	value[0] = CH4_MainValue;
	value[1] = CH5_MainValue;	

}




/***************************************************************************
*函数名：ADC_DMA_Config()
*参数：void
*返回值:void
*功能：ADC的DMA传输配置，内部调用
*****************************************************************************/ 
void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	/*使能DMA1时钟*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	/* DMA1 Channel1 通道1配置*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)ADC1_DR_Address;;                 //指定DMA的外设基地址为ADC1的数据地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_Value[0];                //指定DNA的内存基地址为ADC1转换值存储地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                               //配置DMA传输方向为外设到内存
	DMA_InitStructure.DMA_BufferSize = 2;                                                 //配置缓存大小为1
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                      //失能外设地址增长
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                              //失能内存地址增长
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//DMA_PeripheralDataSize_HalfWord; //配置外设数据大小为半个字长，即16bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//DMA_MemoryDataSize_HalfWord; //配置内存数据大小为半个字长，即16bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                       //配置DMA模式为循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                    //配置优先权为高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                           //初始化DMA2_Stream0

	/* 使能DMA1_Channel1*/
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_Configuration(void)
{  
	unsigned int timeOut =0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	//打开GPIOA时钟和DAC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PT100_PIN1 |PT100_PIN2;	             //设置ADC端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		     //设置IO口模式
	GPIO_Init(PT100_GPIO, &GPIO_InitStructure);				     // PA0,输入时不用设置速率

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 2;	//一路采样

	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);	   // ADC10 规则通道5配置	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_239Cycles5);	   // ADC10 规则通道5配置

	ADC_Cmd(ADC1, ENABLE);	   //使能ADC
	timeOut = 10000;

	ADC_ResetCalibration(ADC1);
	while((ADC_GetResetCalibrationStatus(ADC1)) && (timeOut--));

	timeOut = 10000;
	ADC_StartCalibration(ADC1);
	while((ADC_GetCalibrationStatus(ADC1)) && (timeOut--));

	ADC_ResetCalibration(ADC1);	//使能ADC1 的重设校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1));	//检查ADC1重设校准寄存器结束
	
	ADC_StartCalibration(ADC1);	//开始ADC校准
	while (ADC_GetCalibrationStatus(ADC1));  //检查校准结束	

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	// 使能ADC的DMA方式
	ADC_DMACmd(ADC1, ENABLE);
	ADC_DMA_Config();

	
}




/*******************************************************************************
* Function Name  : DAC_Configuration
* Description    : Configures DAC1 and DAC2
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DAC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	DAC_InitTypeDef DAC_InitStructure;

	//打开GPIOA时钟和DAC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DAC_OUT1_PIN | DAC_OUT2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DAC_OUT_GPIO, &GPIO_InitStructure);	  

	DAC_DeInit();
	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;   //不产生三角波（DAC_Wave_Triangle） 或者 噪声波（DAC_Wave_Noise）
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;     //提高驱动能力可以打开缓冲  但不是rail to rail
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	
	DAC_DMACmd(DAC_Channel_1, DISABLE);     //不使用DMA 
	DAC_DMACmd(DAC_Channel_2, DISABLE);     //不使用DMA 
	
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
}


 

