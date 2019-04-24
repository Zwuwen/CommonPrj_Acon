/**
  ******************************************************************************
  * @file    StepperMotor.c 
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
#include <stdio.h>
#include "StepperMotorInit.h"
#include "StepperMotor.h"


MOTOR StepperMotor[TOTAL_MOTOR_NUMBER];


int PerpareToStepperMotorArea(char *dest)
{
	int *tmp;
	int i;
	tmp = (int*)dest;
	
	for(i=0; i<TOTAL_MOTOR_NUMBER; i++)
	{
		*tmp++ = StepperMotor[i].SpdStart;
		*tmp++ = StepperMotor[i].SpdMax;
		*tmp++ = StepperMotor[i].Acc;
	}
	return 32;
}



void StepperMotorData_FromPersistence(char *src)
{
	int i;
	int *tmp;
	tmp = (int*)src;
	
	if(src != NULL)
	{
		for(i=0; i<TOTAL_MOTOR_NUMBER; ++i)
		{
			StepperMotor[i].SpdStart = *tmp++;
			StepperMotor[i].SpdMax = *tmp++;
			StepperMotor[i].Acc = *tmp++;
		}		
	}
	else
	{
		for(i=0; i<TOTAL_MOTOR_NUMBER; ++i)
		{
			StepperMotor[i].SpdStart = 1000;
			StepperMotor[i].SpdMax = 10000;
			StepperMotor[i].Acc = 10000;			
		}	
	}
}


void StepperMotor_Init(void)
{
	StepperMotor_Initialize_All();
}


void StepperMotor_StateMachine(MOTOR *m)
{
	
}


void StepperMotor_InMainLoop(void)
{
	int i;
	for(i=0; i<TOTAL_MOTOR_NUMBER; i++)
	{
		StepperMotor_StateMachine(&StepperMotor[i]);
	}
}





/**
  * @brief  
  * @param  None
  * @retval None
  */
void ReadEncCount(MOTOR *m)
{
	int c1;
	int c2;
	do
	{
		c1 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
		c2 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
		m->EncodeCurrAbsPos = m->CurrAbsPos; /*record the abspos.*/
	}
	while(((c1>c2) ? c1-c2 : c2-c1) > 2000);
	m->EncodeCount = c2;
	
}	


void ResetEncCount(MOTOR *m)
{	 
	TIM_Cmd(m->EncoderTIM, DISABLE);
	TIM_SetCounter(m->EncoderTIM, 0);
	m->EncodeCircles = 0;
	TIM_Cmd(m->EncoderTIM, ENABLE);	
}



/**
  * @brief  
  * @param  None
  * @retval None
  */
void CalcMotorRunParam_Tcurve(MOTOR *m, int totalPulse)
{
	int timeForAcc, timeForDec;
	int pulseForAcc, pulseForDec;
	
	timeForAcc = ((m->SpdMax - m->SpdStart)*1000) / m->Acc;
	timeForDec = ((m->SpdMax - m->SpdStart)*1000) / m->Dec;
	
	pulseForAcc = (m->SpdStart * timeForAcc + 
				  (m->SpdMax - m->SpdStart)*timeForAcc / 2) /1000;
	pulseForDec = (m->SpdStart * timeForDec + 
				  (m->SpdMax - m->SpdStart)*timeForDec / 2) /1000;
	
	if(totalPulse >= pulseForAcc + pulseForDec)
	{
		m->PulseForAcc = pulseForAcc;
		m->PulseForDec = pulseForDec;
		m->PulseForEqualSpd = totalPulse - pulseForAcc - pulseForDec;
	}
	else
	{
		m->PulseForAcc = totalPulse * m->Dec/(m->Dec + m->Acc);
		m->PulseForDec = totalPulse - m->PulseForAcc;
		m->PulseForEqualSpd = 0;
	}
}




























