/**
  ******************************************************************************
  * @file    StepperMotorInit.c 
  * @author  Bowen.He
  * @version V1.0
  * @created    24-April-2019
  * @lastModified 
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************/
#include "StepperMotorInit.h"



//////////////StepperMotor[0]///////////////////

void PWM_Enable_1(void)
{
	TIM_Cmd(TIM2, ENABLE);	
}


void PWM_Disable_1(void)
{
	TIM_Cmd(TIM2, DISABLE);
}


void Motor_Enable_1(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_5);
}


void Motor_Disable_1(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_5);
}


BOOL GetPositiveSensor_1(void)
{
	return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==0) ? TRUE : FALSE;	
}


BOOL GetNegativeSensor_1(void)
{
	return (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)==0) ? TRUE : FALSE;	
}



void SetMotorDir_1(_MOTORDIR Dir)
{
	(Dir==DIR_POSITIVE)? GPIO_ResetBits(GPIOB, GPIO_Pin_0): 
							GPIO_SetBits(GPIOB, GPIO_Pin_0);
}



BOOL PWM_Pulse_Change_1(int Freqs)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	u16 Duty_Ratio = 50;  /*50%*/

	if(Freqs > 100000 || Freqs < 100 )
		return FALSE;
	
	TIM_Prescaler = (u16)((SystemCoreClock / Freqs) / 65535);
	TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / Freqs);
	TIM_Pulse = (u32)TIM_Period * Duty_Ratio / 100;
	
	TIM2->ARR = TIM_Period;
	TIM2->PSC = TIM_Prescaler;
	TIM2->CCR1 = TIM_Pulse;
	
	return TRUE;
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
BOOL PWM_OCPulse_Init_1(int Freqs)
{
	u16 PrescalerValue;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  		 TIM_OCInitStructure;
	u16 Duty_Ratio = 50;	

	if(Freqs > 100000 || Freqs < 100)
		return FALSE;

	TIM_UpdateRequestConfig(TIM2, ENABLE);
	
	PrescalerValue = (u16)((SystemCoreClock / Freqs) / 65535);
	
    TIM_TimeBaseStructure.TIM_Period = (u16)(SystemCoreClock / (PrescalerValue + 1) / Freqs);
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);		
	
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = (u32)TIM_TimeBaseStructure.TIM_Period * Duty_Ratio / 100;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_GenerateEvent(TIM2, TIM_EventSource_Update);
	return TRUE;
}


/**
  * @brief  
  * @param  
  * @retval res
  */
void StepperMotor_0_Encoder_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  	TIM_ICInitTypeDef TIM_ICInitStructure;
	
	/////////////////////GPIO RCC/////////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//TI1 TI2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//z
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	///////////////////NVIC///////////////////////////
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	///////////////////TIM3////////////////////////////////
	TIM_DeInit(TIM3);							   					//ENCODER_TIMER复位
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;  								// 无分频
	
	TIM_TimeBaseStructure.TIM_Period = ( 4 * StepperMotor[0].sEncodePPR)-1; 			//计数器重载值
	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				//向上计数模式   
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);	//编码器接口初始化

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6; 									//设置滤波器    (0x00~0x0f之间)
	TIM_ICInit(TIM3, &TIM_ICInitStructure);		   

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_SetCounter(TIM3, 0);

	TIM_Cmd(TIM3, ENABLE);  
	
}


/**
  * @brief  
  * @param  
  * @retval res
  */
void StepperMotor_0_Hardware_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/////////////////////GPIO RCC/////////////////////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
						   RCC_APB2Periph_GPIOB | 
						   RCC_APB2Periph_GPIOC	|
						   RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//Dir
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//Enable
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure); 

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15; //Input 0 1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0; //Clk
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	///////////////////NVIC///////////////////////////
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	///////////////////////////////////////////////////
	
}



void StepperMotor_0_Initialize(void)
{
	StepperMotor_0_Hardware_Init();
	PWM_OCPulse_Init_1(1000);
	StepperMotor_0_Encoder_Init();
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	
	
	StepperMotor[0].EncoderTIM = TIM3;
	StepperMotor[0].PWM_Disable = PWM_Disable_1;
	StepperMotor[0].PWM_Enable = PWM_Enable_1;
	StepperMotor[0].Motor_Enable = Motor_Enable_1;
	StepperMotor[0].Motor_Disable = Motor_Disable_1;
	StepperMotor[0].GetPositiveSensor = GetPositiveSensor_1;
	StepperMotor[0].GetNegativeSensor = GetNegativeSensor_1;
	StepperMotor[0].SetMotorDir = SetMotorDir_1;
	StepperMotor[0].PWM_Pulse_Change = PWM_Pulse_Change_1;
	
	StepperMotor[0].Motor_Enable();
	
}



void StepperMotor_Initialize_All(void)
{
	StepperMotor_0_Initialize();

}








