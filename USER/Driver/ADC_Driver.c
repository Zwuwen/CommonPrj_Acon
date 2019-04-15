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
  * @brief  ��ʼ��ADC1 ����
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
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				// PC0,����ʱ������������
}

/**
  * @brief  ����ADC1 DMA1 ��У��ADC1���������ADC1
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
  * @brief  ��ʼ��AD����ģ��ADC1
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
  * @retval ADC1_CH2_MainValue ADC������ֵ
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
*��������ADC_DMA_Config()
*������void
*����ֵ:void
*���ܣ�ADC��DMA�������ã��ڲ�����
*****************************************************************************/ 
void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	/*ʹ��DMA1ʱ��*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	/* DMA1 Channel1 ͨ��1����*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)ADC1_DR_Address;;                 //ָ��DMA���������ַΪADC1�����ݵ�ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC1_Value[0];                //ָ��DNA���ڴ����ַΪADC1ת��ֵ�洢��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                               //����DMA���䷽��Ϊ���赽�ڴ�
	DMA_InitStructure.DMA_BufferSize = 2;                                                 //���û����СΪ1
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                      //ʧ�������ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                              //ʧ���ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//DMA_PeripheralDataSize_HalfWord; //�����������ݴ�СΪ����ֳ�����16bit
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//DMA_MemoryDataSize_HalfWord; //�����ڴ����ݴ�СΪ����ֳ�����16bit
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                       //����DMAģʽΪѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                    //��������ȨΪ��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);                                           //��ʼ��DMA2_Stream0

	/* ʹ��DMA1_Channel1*/
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_Configuration(void)
{  
	unsigned int timeOut =0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	//��GPIOAʱ�Ӻ�DACʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = PT100_PIN1 |PT100_PIN2;	             //����ADC�˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		     //����IO��ģʽ
	GPIO_Init(PT100_GPIO, &GPIO_InitStructure);				     // PA0,����ʱ������������

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //�Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 2;	//һ·����

	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);	   // ADC10 ����ͨ��5����	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_239Cycles5);	   // ADC10 ����ͨ��5����

	ADC_Cmd(ADC1, ENABLE);	   //ʹ��ADC
	timeOut = 10000;

	ADC_ResetCalibration(ADC1);
	while((ADC_GetResetCalibrationStatus(ADC1)) && (timeOut--));

	timeOut = 10000;
	ADC_StartCalibration(ADC1);
	while((ADC_GetCalibrationStatus(ADC1)) && (timeOut--));

	ADC_ResetCalibration(ADC1);	//ʹ��ADC1 ������У׼�Ĵ���
	while (ADC_GetResetCalibrationStatus(ADC1));	//���ADC1����У׼�Ĵ�������
	
	ADC_StartCalibration(ADC1);	//��ʼADCУ׼
	while (ADC_GetCalibrationStatus(ADC1));  //���У׼����	

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
	// ʹ��ADC��DMA��ʽ
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

	//��GPIOAʱ�Ӻ�DACʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = DAC_OUT1_PIN | DAC_OUT2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DAC_OUT_GPIO, &GPIO_InitStructure);	  

	DAC_DeInit();
	DAC_StructInit(&DAC_InitStructure);
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;   //���������ǲ���DAC_Wave_Triangle�� ���� ��������DAC_Wave_Noise��
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable ;     //��������������Դ򿪻���  ������rail to rail
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	
	DAC_DMACmd(DAC_Channel_1, DISABLE);     //��ʹ��DMA 
	DAC_DMACmd(DAC_Channel_2, DISABLE);     //��ʹ��DMA 
	
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,0);
	DAC_SetChannel2Data(DAC_Align_12b_R,0);
}


 

