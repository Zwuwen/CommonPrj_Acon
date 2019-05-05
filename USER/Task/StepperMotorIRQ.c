/**
  ******************************************************************************
  * @file    StepperMotorIRQ.c 
  * @author  Bowen.He
  * @version V1.0
  * @created    25-April-2019
  * @lastModified 
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************/
 #include "StepperMotorIRQ.h" 
 #include "StepperMotorInit.h"

void StepperMotor_Encode_IRQHandler(MOTOR *m);
void StepperMotor_PWM_IRQHandler(MOTOR *m);


void TIM2_IRQHandler(void)
{  
	StepperMotor_PWM_IRQHandler(&StepperMotor[0]);
}



void TIM3_IRQHandler(void)
{
	StepperMotor_Encode_IRQHandler(&StepperMotor[0]);	
}





void StepperMotor_Encode_IRQHandler(MOTOR *m)
{
	if (TIM_GetITStatus(m->EncoderTIM, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(m->EncoderTIM, TIM_IT_Update);	

		if((m->EncoderTIM->CR1 & (0x01<<4))  == 0)
			m->EncodeCircles++;	 //顺时针旋转		 
		else
			m->EncodeCircles--;	 //逆时针旋转	

		if((m->EncodeCircles > 500000) || 
		   (m->EncodeCircles < -500000))
		{
			m->EncodeCircles = 0;	
		}
	}	
}



void StepperMotor_PWM_IRQHandler(MOTOR *m)
{
	if(TIM_GetITStatus(m->PwmTIM, TIM_IT_Update) != RESET)
	{
		m->PulseEscape++;
		if(m->Dir == DIR_POSITIVE)
			m->CurrAbsPos++;
		else 
			m->CurrAbsPos--;
		
		if((m->RunMode == MODE_POSITION) &&			/*定位模式下，走完全程*/
			(m->PulseEscape == m->TotalPulse))
		{
			m->ActionCode = PASS;
			m->RunState = RUN_STOP_IMMEDIATELY;
		}
		
		/*两边的极限位传感器*/
		if( CHECK_LIMIT_SENSOR_SIGNAL(m) )
		{
			m->ActionCode = ERR_LIMITSENSOR;
			m->RunState = RUN_STOP_IMMEDIATELY;
		}
		
		if((m->Dir == DIR_POSITIVE)&&
			(m->RunMode == MODE_POSITION) &&
			(m->CurrAbsPos >= m->SoftLimitP))
		{
			m->ActionCode = ERR_OVERSOFTLIMIT;
			m->RunState = RUN_STOP_IMMEDIATELY;
		}
		
		if(m->RunState == RUN_STOP_IMMEDIATELY)
		{
			m->PWM_Disable();
			m->flag.Bit.enablePwmAdjust = 0;
			m->flag.Bit.taskCompletion = 1;
			m->RunState = RUN_OVER;
			m->PulseEscape = 0;
		}
		
		TIM_ClearITPendingBit(m->PwmTIM, TIM_IT_Update);
	}	

}












