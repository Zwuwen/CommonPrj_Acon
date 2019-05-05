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
#include "AIA_ErrorCode.h"

MOTOR StepperMotor[TOTAL_MOTOR_NUMBER];

////////////Function used in this file////////////////////////////
BOOL ReadEncCount(MOTOR *m);
void ChangeRunStateByEncoder(MOTOR *m);

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
			StepperMotor[i].CurrSpd = 1000;	
			StepperMotor[i].SpdRst = 1000;	
			StepperMotor[i].Current = 80;
			StepperMotor[i].EncodeErrOffset = 2000;
			StepperMotor[i].OriginOffset = 100;
			
			StepperMotor[i].RunState = RUN_OVER;
			StepperMotor[i].OriginState = NO_ORIGIN;
			StepperMotor[i].RunMode = MODE_POSITION;

			StepperMotor[i].flag.Bit.HasEncoder = 0;	
			StepperMotor[i].OutofStepMode = RUN_STOP_WHEN_OUTOFSTEP;
			StepperMotor[i].MoveType = MOVETYPE_LINE;
			StepperMotor[i].MicroType = MICRO8;
			StepperMotor[i].StopMode = STOPIMMEDIATELY;		
			StepperMotor[i].driverCurrent = MOTOR_DRIVE_2_0_A;
			StepperMotor[i].AccMode = T_CURVE;
			
			StepperMotor[i].sEncodePPR = ENCODER_PPR;
			StepperMotor[i].sMicroStep = StepperMotor[i].MicroType;
			
		}	
	}
	
	for(i=0; i<TOTAL_MOTOR_NUMBER; ++i)
	{
		StepperMotor[i].PulseEncodeRatio = ((StepperMotor[i].sMicroStep*200.0f)/(StepperMotor[i].sEncodePPR*4.0f));		
	}
}




void StepperMotor_Init(void)
{
	StepperMotor_Initialize_All();
}

void updateRealPosByEncode(MOTOR *m)
{
	float tmp;
	//m->CurrAbsPos = (m->EncodeCount/(m->sEncodePPR*4)) * (m->sMicroStep*200);
	//m->CurrAbsPos = m->EncodeCount * ((m->sMicroStep*200)/(m->sEncodePPR*4));
	//m->CurrAbsPos = m->EncodeCount * m->PulseEncodeRatio;
	tmp = m->EncodeCount;
	tmp *= m->PulseEncodeRatio;
	m->CurrAbsPos = tmp;
}



/**
  * @brief  
  * @param  None
  * @retval None
  */
void MotorChangeRunState(MOTOR *m)
{
	if((m->RunState == RUN_STOP_IMMEDIATELY)||
	   (m->RunState ==  RUN_OVER))	return;
	
	switch(m->RunMode)
	{
		case MODE_EQUALSPD:	/*匀速模式*/
			if(m->RunState == RUN_ACCSPD)
			{
				if(m->CurrSpd >= m->SpdMax)
					m->RunState = RUN_EQUALSPD;
			}
		break;
		case MODE_POSITION: /*定位模式*/
			if((m->RunState == RUN_ACCSPD) &&
			   (m->PulseEscape >= m->PulseForAcc)) /*加速转匀速*/
			   m->RunState = RUN_EQUALSPD;
			if((m->RunState == RUN_EQUALSPD) &&    /*匀速转减速*/
			   (m->PulseEscape >= m->PulseForAcc + m->PulseForEqualSpd))
			   m->RunState = RUN_DECSPD;
		break;		
	}
	
	if((m->RunState == RUN_STOP_BYDEC) &&  /*减速停止过程*/
	   (m->CurrSpd <= m->SpdStart))
	   m->RunState = RUN_STOP_IMMEDIATELY;
	
	if((m->flag.Bit.stopByCmd == 1) &&	/*脉冲发完，或者命令发送到停止*/
	   (m->RunState != RUN_STOP_IMMEDIATELY))
	{
		m->flag.Bit.stopByCmd = 0;
		m->ActionCode = ERR_STOP;
		m->RunState = (m->StopMode == STOPBYDEC) ? RUN_STOP_BYDEC : 
												   RUN_STOP_IMMEDIATELY;
	}
	/*编码器判断*/
	if((m->flag.Bit.HasEncoder == 1) &&
	   (m->RunState != RUN_STOP_BYDEC))
	{
		ChangeRunStateByEncoder(m);		
    }	
}















/**
  * @brief  
  * @param  None
  * @retval None
  */
void MotorHandleRunState(MOTOR *m)
{
	switch(m->RunState)
	{
		case RUN_ACCSPD:
			m->CurrSpd += m->Acc /1000;
			if(m->CurrSpd > m->SpdMax) m->CurrSpd = m->SpdMax;
			m->PWM_Pulse_Change(m->CurrSpd);
		break;
		case RUN_DECSPD:  /*减速*/
		case RUN_STOP_BYDEC: /*减速停止*/
			m->CurrSpd -= m->Dec /1000;
			if(m->CurrSpd < m->SpdStart ) m->CurrSpd = m->SpdStart;
			m->PWM_Pulse_Change(m->CurrSpd);
		case RUN_OVER:
			//按照编码器的值修正当前的绝对脉冲	
		default: break;
	}
}



//_MOTORSTATEMACHINE Motor_SM_Mode_Equalspd(MOTOR *m)
//{
//	switch(m->Step)
//	{
//		case STEP_CALCULATE:
//			if((m->flag.Bit.HasEncoder == 1) && (ReadEncCount(m) == FALSE)) 
//			{
//				/*retry ReadEncCount*/
//				break;				
//			}
//			if(m->flag.Bit.HasEncoder == 1)
//			{
//				updateRealPosByEncode(m);
//			}
//			
//			m->flag.Bit.time1ms = 0;
//			m->flag.Bit.firstEnablePwm = 1;
//			m->RunState = RUN_ACCSPD;
//			m->CurrSpd = m->SpdStart;
//			m->PWM_Pulse_Change(m->CurrSpd);
//			
//		break;
//			
//		case STEP_MOTORRUN:
//			if(m->flag.Bit.time1ms == 0) break;
//		
//			m->flag.Bit.time1ms = 0;
//		
//			if(m->flag.Bit.firstEnablePwm)
//			{
//				m->flag.Bit.firstEnablePwm = 0;
//				m->PWM_Enable();
//			}
//			
//			
//			MotorChangeRunState(m);
//			MotorHandleRunState(m);	
//		break;
//		
//		default:break;
//	}
//}


//_MOTORSTATEMACHINE Motor_SM_Mode_Poistion(MOTOR *m)
//{

//}


//_MOTORSTATEMACHINE Motor_SM_Action_Complete(MOTOR *m)
//{

//}



void StepperMotor_StateMachine(MOTOR *m)
{
//	switch(m->StateMachine)
//	{
//		case SM_MODE_EQUALSPD:
//			m->StateMachine = Motor_SM_Mode_Equalspd(m);
//		break;
//		
//		case SM_MODE_POSITION:
//			m->StateMachine = Motor_SM_Mode_Poistion(m);
//		break;
//				
//		case SM_ACTION_COMPLETE:
//			m->StateMachine = Motor_SM_Action_Complete(m);
//		break;
//		
//		default:break;
//	}
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
/*中断里 1ms 调用一次*/
void MotorAdjustRunningParamPerMS(MOTOR *m)
{
	if(m->flag.Bit.enablePwmAdjust == 1)
	{
		if(m->flag.Bit.firstEnablePwm)
		{
			m->flag.Bit.firstEnablePwm = 0;
			m->PWM_Enable();
		}
		if(m->AccMode == T_CURVE)
		{
			MotorChangeRunState(m);
			MotorHandleRunState(m);
		}
	}
}


void StepperMotor_InIrq(void)
{
	int i;
	for(i=0; i<TOTAL_MOTOR_NUMBER; i++)
	{
		MotorAdjustRunningParamPerMS(&StepperMotor[i]);
	}
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
BOOL ReadEncCount(MOTOR *m)
{
	int c1;
	int c2;
	
//	c1 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
//	c2 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
//	m->EncodeCurrAbsPos = m->CurrAbsPos; /*record the abspos.*/

//	if(((c1>c2) ? c1-c2 : c2-c1) > 2000)
//	{
//		return FALSE;
//	}
//	else
//	{
//		m->EncodeCount = c2;
//		return TRUE;
//	}
	
	do
	{
		c1 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
		c2 = m->sEncodePPR * 4 * m->EncodeCircles + m->EncoderTIM->CNT;
		m->EncodeCurrAbsPos = m->CurrAbsPos; /*record the abspos.*/
	}
	while(((c1>c2) ? c1-c2 : c2-c1) > 2000); 
	m->EncodeCount = c2;
	
	return TRUE;
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






/**
  * @brief  
  * @param  None
  * @retval None
  */
void CalcMotorRunParam(MOTOR *m, int totalPulse)
{
	if(m->AccMode == T_CURVE)
		CalcMotorRunParam_Tcurve(m, totalPulse);
		
}

/**
  * @brief  
  * @param  None
  * @retval None
  */
void ChangeRunStateByEncoder(MOTOR *m)
{
	int calcEncodeCount;
	int encodeGap;
	float tmp;
	
	ReadEncCount(m);
	
	tmp = m->EncodeCurrAbsPos;
	tmp /= m->PulseEncodeRatio;
	calcEncodeCount = tmp;
	
	encodeGap = (m->EncodeCount > calcEncodeCount) ? (m->EncodeCount - calcEncodeCount):
													 (calcEncodeCount-m->EncodeCount);
	
	if(encodeGap >= m->EncodeErrOffset)
	{
		m->ActionCode = ERR_STEPLOSS;
		
		if(m->StopMode == STOPIMMEDIATELY)
			m->RunState = RUN_STOP_IMMEDIATELY;
		else if(m->StopMode == STOPBYDEC)
			m->RunState = RUN_STOP_BYDEC;
	}
}


/**
  * @brief  
  * @param  None
  * @retval None
  */
int MotorGoto(MOTOR *m, int totalPulse, _MOTORABSORREL absOrRel)
{
	int totalPulseNum;
	totalPulseNum = totalPulse;
	m->RunMode = MODE_POSITION;
	m->ActionCode = PASS;
	
	if(absOrRel == ABS_COORD)
	{
		m->PosPurpose = totalPulseNum;
		totalPulseNum = totalPulseNum - m->CurrAbsPos;
	}
	else
	{
		m->PosPurpose = totalPulseNum + m->CurrAbsPos;
	}
		
	/*根据相对坐标计算电机方向以及移动的脉冲总数*/
	if(totalPulseNum > 0)
	{
		m->Dir = DIR_POSITIVE;
		m->SetMotorDir(DIR_POSITIVE);
		m->TotalPulse = totalPulseNum;
	}
	else if(totalPulseNum < 0)
	{
		m->Dir = DIR_NEGATIVE;
		m->SetMotorDir(DIR_NEGATIVE);
		totalPulseNum = -totalPulseNum;
		m->TotalPulse = totalPulseNum;
	}
	else /*==0*/
	{
		return m->ActionCode = PASS;
	}
	
	if( CHECK_LIMIT_SENSOR_SIGNAL(m) )
	{
		return m->ActionCode = ERR_LIMITSENSOR; 
	}
	
	if((m->Dir == DIR_POSITIVE)&&
		(m->CurrAbsPos >= m->SoftLimitP))
	{
		return m->ActionCode = ERR_OVERSOFTLIMIT;
	}
	
	CalcMotorRunParam(m, totalPulseNum);
	m->RunState = RUN_ACCSPD;
	
	m->CurrSpd = m->SpdStart;
	m->PWM_Pulse_Change(m->CurrSpd);
	m->flag.Bit.firstEnablePwm = 1;
	m->flag.Bit.enablePwmAdjust = 1;
	m->flag.Bit.taskCompletion = 0;
	while(m->flag.Bit.taskCompletion == 0);
	return m->ActionCode;
}


int MotorGotoA(MOTOR *m, int totalPulse, _MOTORABSORREL absOrRel)
{
	int ret;
	int backupspd;
	
	if(m->flag.Bit.HasEncoder == 0)
		return MotorGoto(m, totalPulse, absOrRel);
		
	/*hasEncoder  calc the current positon by encoder first*/
	ReadEncCount(m);
	updateRealPosByEncode(m);
	

	if(m->OutofStepMode == RUN_STOP_WHEN_OUTOFSTEP)
	{
		ret = MotorGoto(m, totalPulse, absOrRel);
		/*有可能编码器发现有失步，但是没有超过判断失步的阀值，
		  运行返回还是正确，但是定位稍微有一些偏离，所以再走一次，然后判断返回值
		*/
		if((ret == PASS) &&
			((m->PosPurpose - m->CurrAbsPos > 2) ||
			(m->PosPurpose - m->CurrAbsPos < -2)))
		{
			backupspd = m->SpdMax;
			m->SpdMax = m->SpdStart;
			ret = MotorGoto(m, m->PosPurpose, ABS_COORD);
			m->SpdMax = backupspd;
			
			if((ret == PASS) &&
			((m->PosPurpose - m->CurrAbsPos > 2) ||
			(m->PosPurpose - m->CurrAbsPos < -2)))
				ret = PASS;
			else 
				ret = ERR_STEPLOSS;
			
			if(ret == PASS)
				m->CurrAbsPos = m->PosPurpose;
			
			return m->ActionCode = ret;	
		}
	}
	else if(m->OutofStepMode == RUN_ANYWAY_WHEN_OUTOFSTEP)
	{	/*在RUN_ANYWAY_WHEN_TROUBLE模式下运行，因为始终要努力走到目标位置，
		所以用ABS_COORD方式运行，先计算Motor.PosPurpose*/
		m->PosPurpose = (absOrRel == ABS_COORD) ? totalPulse : m->CurrAbsPos+totalPulse;
		
		do
		{
			ret = MotorGoto(m, m->PosPurpose, ABS_COORD);
			if((ret != PASS) && (ret != ERR_STEPLOSS))
				return m->ActionCode = ret;
		}while((m->PosPurpose - m->CurrAbsPos > 2) ||
			   (m->PosPurpose - m->CurrAbsPos < -2));
			   
		m->CurrAbsPos = m->PosPurpose;
		return m->ActionCode = PASS;	
	}
	return m->ActionCode = ret;
}



/**
  * @brief  
  * @param  None
  * @retval None
  */
int MotorRun(MOTOR *m, _MOTORDIR Dir)
{
	m->Dir = Dir;
	m->RunMode = MODE_EQUALSPD;
	m->SetMotorDir(m->Dir);
	
	m->RunState = RUN_ACCSPD;
	m->CurrSpd = m->SpdStart;
	m->PWM_Pulse_Change(m->CurrSpd);
	m->flag.Bit.firstEnablePwm = 1;
	m->flag.Bit.enablePwmAdjust = 1;
	m->flag.Bit.taskCompletion = 0;
	while(m->flag.Bit.taskCompletion == 0);
	return m->ActionCode;	

}


/**
  * @brief  if current position is in the origin positon, move to postive, 
  *			and find origin again.
  *			Note: FINDBYZSIG FINDBYTTL mode is special
  * @param  None
  * @retval None
  */
int MoveBeforeFindOrigin(MOTOR *m, _RSTMODE rstmode)
{
	int ret;
	int tryTime;
	
	tryTime = 0;
	if((rstmode != FINDBYZSIG) && (rstmode != FINDBYTTL))
	{
		while( GET_NEGATIVESENSOR_STATE(m) ) /*maybe the current position is in the origin positon*/
		{
			ret = MotorGotoA(m, m->OriginOffset, REL_COORD); /*move away: dir POSTIVE*/
			if(ret != PASS) 
				return m->ActionCode = ret;
			DELAY_MS(50);
			tryTime++;
			if(tryTime > 5)
				return m->ActionCode = ERR_ORIGINSENSOR;
		}
	}
	else 
	{
		if(rstmode == FINDBYZSIG)
		{
			while(GET_ZSIGNAL_STATE(m))
			{
				ret = MotorGotoA(m, -m->OriginOffset, REL_COORD);
				if(ret != PASS) return m->ActionCode = ret;
			}
				
		}
		if(rstmode == FINDBYTTL)
		{
			;
		}	
	}
	return PASS;
}




/**
  * @brief  Find the Origin. After this Step, the absmove will be allowed.
  *			Otherwise, the motor only response the relmove cmd.
  * @param  None
  * @retval None
  */
int DiffRstModeFindOrigin(MOTOR *m, _MOTORDIR dir, _RSTMODE rstmode)
{
	int ret;
	int backupErrOffset;
	
	if(m->flag.Bit.HasEncoder == 1)
	{
		/*hasEncoder  calc the current positon by encoder first*/
		ReadEncCount(m);
		updateRealPosByEncode(m);
	}
	
	switch(rstmode)
	{
		case FINDBYSENSOR:
			ret = MotorRun(m, DIR_NEGATIVE);
			if(ret != ERR_LIMITSENSOR)
				return m->ActionCode = ret;
			DELAY_MS(500);
			ret = MotorGotoA(m, m->OriginOffset, REL_COORD);
			if(ret != PASS) 
				return m->ActionCode = ret;	
		break;
		
		case FINDBYBLOCK:
			ret = MotorRun(m, DIR_NEGATIVE);
			if(ret != ERR_STEPLOSS) 
				return m->ActionCode = ret;
			
			DELAY_MS(50);
//				Wait_ms(30);
//				MotorSetCurrentByDAC(HOLDCURRENT);
//				Wait_ms(300);
			backupErrOffset = m->EncodeErrOffset;
			m->EncodeErrOffset *= 3;
			ret = MotorGotoA(m, -m->OriginOffset, REL_COORD);
			m->EncodeErrOffset = backupErrOffset;
			
			//MotorSetCurrentByDAC(HOLDCURRENT);
			if(ret != PASS) return m->ActionCode = ret;			
		break;
		
		case FINDBYZSIG:
		break;
		
		case FINDBYTTL:
		break;
		
		default:break;
	}
	return m->ActionCode = PASS;
}


/**
  * @brief  Find the Origin. After this Step, the absmove will be allowed.
  *			Otherwise, the motor only response the relmove cmd.
  * @param  None
  * @retval None
  */
int MotorFindOriginGo(MOTOR *m, _MOTORDIR dir, _RSTMODE rstmode)
{
	int ret;
	
	if((m->MoveType == MOVETYPE_ROTATE) &&
	   (rstmode != FINDBYBLOCK && rstmode != FINDBYTTL))
		return ERR_RSTMODE;
	if((m->MoveType == MOVETYPE_LINE) &&   
	   (rstmode != FINDBYSENSOR && rstmode != FINDBYBLOCK))
	   return ERR_RSTMODE;
	   

	if((rstmode == FINDBYZSIG) || (rstmode == FINDBYTTL))
	{
		m->flag.Bit.EnableSensorLimitN = 0;
		m->flag.Bit.EnableSensorLimitP = 0;
	}
	else m->flag.Bit.EnableSensorLimitN = 1;
	
	if(dir == DIR_NEGATIVE)
	{
		ret = MoveBeforeFindOrigin(m, rstmode); /*avoid a situation that the current Position is origin.*/
		if(ret != PASS) 
			return m->ActionCode = ret;
		
		DELAY_MS(50);
		
		ret = DiffRstModeFindOrigin(m, dir, rstmode);
		if(ret != PASS) 
			return m->ActionCode = ret;
	}
	return m->ActionCode = PASS;
}





/**
  * @brief  Find the Origin. After this Step, the absmove will be allowed.
  *			Otherwise, the motor only response the relmove cmd.
  * @param  None
  * @retval None
  */
int MotorFindOrigin(MOTOR *m, _RSTMODE rstmode)
{
	int ret;
	int bkupSpdMax,bkupSpdStart;
	bkupSpdMax = m->SpdMax;
	bkupSpdStart = m->SpdStart;
	m->SpdMax = m->SpdRst;
	m->SpdStart = m->SpdRst;
	
	m->OriginState = NO_ORIGIN;
	
	m->Set_Current(RST_CURRENT);
	
	ret = MotorFindOriginGo(m, DIR_NEGATIVE, rstmode);
	
	m->Set_Current(IDLE_CURRENT);	
	
	m->SpdMax = bkupSpdMax;
	m->SpdStart = bkupSpdStart;	
	
	if(ret == PASS)
	{
		m->OriginState = OVER_ORIGIN;
		m->CurrAbsPos = 0;
		m->PosPurpose = 0;
		ResetEncCount(m);
	}
	return m->ActionCode = ret;	
}






