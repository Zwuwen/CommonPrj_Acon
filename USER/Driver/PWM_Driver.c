/**
  ******************************************************************************
  * @file    PWM_Driver.c
  * @author  AIA Team
  * @version V1.0
  * @created    15-April-2019
  * @lastModified 15-April-2019
  * @brief   PWM Driver
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/


#include "PWM_Driver.h"

/**
  * @brief  config PWM timer2   .
  * @param  None
  * @retval None
  */
void PWM_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	u32 Frequency = 20000; 
	u8 Duty_Ratio = 0;   
	u16 PrescalerValue;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	PrescalerValue = (u16)((SystemCoreClock / Frequency) / 65535);

	TIM_TimeBaseStructure.TIM_Period = (u16)(SystemCoreClock / (PrescalerValue + 1) / Frequency);
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM2, ENABLE);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);	
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (u32)TIM_TimeBaseStructure.TIM_Period * Duty_Ratio / 10000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_CtrlPWMOutputs(TIM2, ENABLE);
}

/**
  * @brief  change pwm duty and frequency  .
  * @param  None
  * @retval None
  */

u8 PWM_Change(u32 _Frequency, u16 _DutyRatio, u8 _ucChannel)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	
	TIM_Prescaler = (u16)((SystemCoreClock / _Frequency) / 65535);

    TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / _Frequency);
    TIM_Pulse = (u32)TIM_Period * _DutyRatio / 100;
	
	TIM2->ARR = TIM_Period ; 
  	/* Set the Prescaler value */
  	TIM2->PSC = TIM_Prescaler; 
	
	/* Set the Capture Compare Register value */ 		
  	if(_ucChannel==1) 
	{
		TIM2->CCR1 = TIM_Pulse;
  	}
	else if(_ucChannel==2) 
	{
		TIM2->CCR2 = TIM_Pulse;	 
	}
	else if(_ucChannel==3)
	{
		TIM2->CCR3 = TIM_Pulse;  
	}
	else if(_ucChannel==4)
	{
		TIM2->CCR4 = TIM_Pulse; 
	} 
	return 0;
}

/**
  * @brief  change ch1 duty circle   .
  * @param  None
  * @retval None
  */

u8 PWM_Ch1DutyChange(u16 Duty_Ratio)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	u32 Frequency = 20000;		
	TIM_Prescaler = (u16)((SystemCoreClock / Frequency) / 65535);

    TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / Frequency);
    TIM_Pulse = (u32)TIM_Period * Duty_Ratio / 10000;
	
	TIM2->ARR = TIM_Period ; 
  	/* Set the Prescaler value */
  	TIM2->PSC = TIM_Prescaler; 
	/* Set the Capture Compare Register value */ 		
  	TIM2->CCR1 = TIM_Pulse;	 
	return 0;
}


/**
  * @brief  change ch2 duty circle   .
  * @param  None
  * @retval None
  */

u8 PWM_Ch2DutyChange(u16 Duty_Ratio)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	u32 Frequency = 20000;		
	TIM_Prescaler = (u16)((SystemCoreClock / Frequency) / 65535);

    TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / Frequency);
    TIM_Pulse = (u32)TIM_Period * Duty_Ratio / 10000;
	
	TIM2->ARR = TIM_Period ; 
  	/* Set the Prescaler value */
  	TIM2->PSC = TIM_Prescaler; 
	/* Set the Capture Compare Register value */ 		
  	TIM2->CCR2 = TIM_Pulse;	 
	return 0;
}

/**
  * @brief  change ch3 duty circle   .
  * @param  None
  * @retval None
  */

u8 PWM_Ch3DutyChange(u16 Duty_Ratio)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	u32 Frequency = 20000;		
	
	TIM_Prescaler = (u16)((SystemCoreClock / Frequency) / 65535);

    TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / Frequency);
    TIM_Pulse = (u32)TIM_Period * Duty_Ratio / 10000;
	
	TIM2->ARR = TIM_Period ; 
  	/* Set the Prescaler value */
  	TIM2->PSC = TIM_Prescaler; 
	/* Set the Capture Compare Register value */ 		
  	TIM2->CCR3 = TIM_Pulse;	 
	return 0;
}

/**
  * @brief  change ch4 duty circle .
  * @param  None
  * @retval None
  */
u8 PWM_Ch4DutyChange(u16 Duty_Ratio)
{
	u16 TIM_Prescaler;
	u16 TIM_Pulse;
	u16 TIM_Period;
	u32 Frequency = 20000;		
	TIM_Prescaler = (u16)((SystemCoreClock / Frequency) / 65535);

    TIM_Period = (u16)(SystemCoreClock / (TIM_Prescaler + 1) / Frequency);
    TIM_Pulse = (u32)TIM_Period * Duty_Ratio / 10000;
	
	TIM2->ARR = TIM_Period ; 
  	/* Set the Prescaler value */
  	TIM2->PSC = TIM_Prescaler; 
	/* Set the Capture Compare Register value */ 		
  	TIM2->CCR4 = TIM_Pulse;	 
	return 0;
}

/**
  * @brief  Enable PWM  .
  * @param  None
  * @retval None
  */
void PWM_Enable(void)
{
  	TIM_Cmd(TIM2, ENABLE);	
}

/**
  * @brief  Disable PWM  .
  * @param  None
  * @retval None
  */
void PWM_Disable(void)
{
   	TIM_Cmd(TIM2, DISABLE);
}
