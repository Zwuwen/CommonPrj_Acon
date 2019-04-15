/**
  ******************************************************************************
  * @file    AIA_PID.c
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
  * @lastModify 25-March-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  * Proportional-Integral-Derivative (PID) control
  * Implements a PID controller using a PID algorithm for simple PID applications
  * or high speed control applications that require an efficient algorithm. The PID 
  * algorithm features control output range limiting with integrator anti-windup 
  * and bumpless controller output for PID gain changes.
  *
  * A typical PID lib contains 2 files. There are one core file: AIA_PID.c,
  * one configuration file: AIA_PID.h
  * refer to : NI_PID_pid.lvlib:PID.vi
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
#include "AIA_PID.h"
#include "AIA_Utilities.h"
#include "AIA_ErrorCode.h"
#include "AIA_Protocol2.0.h"
#include "CAN_Driver.h"

#include "stdio.h"
#include "string.h"

_LVPIDPARAM LVPID[TOTAL_PID_NUMBER];

/**
  * @brief  Derivative Control		kd
  * @param  *lvpid   LVPID struct point
  * @retval None
  */
static void LVPID_Derivative_Control(_LVPIDPARAM *lvpid)
{
	long Calc_tmp;
	if(lvpid->flag.Bit.init == 1)
	{
		lvpid->DA = 0;	
	}
	else
	{
		if(lvpid->dt > 0)  
		{
			/* Kd: (PV_Last - PV) * (Td * Kp) / (dt_min)     
			   dt_min: dt(s)/60 */
			Calc_tmp = lvpid->PV_Last - lvpid->PV;
			Calc_tmp *= lvpid->Kp;
			Calc_tmp *= lvpid->Td;
			Calc_tmp *= 60;
			Calc_tmp /= lvpid->dt;
			Calc_tmp /= KpRATIO;
			Calc_tmp /= KpRATIO;
			lvpid->DA = Calc_tmp;
		}
		else /* dt = 0 min*/
		{
			lvpid->DA = lvpid->DA_Last;
			lvpid->PV = lvpid->PV_Last;
		}
	}
}





/**
  * @brief  Proportional Control   Kp
  * @param  *lvpid   LVPID struct point
  * @retval None
  */
static void LVPID_Proportional_Control(_LVPIDPARAM *lvpid)
{
	int Calc_tmp;
	Calc_tmp = lvpid->Kp;
	Calc_tmp *= lvpid->Err;
	Calc_tmp /= KpRATIO;
	lvpid->PA = Calc_tmp;
}




/**
  * @brief  Integral Control	Ki
  * @param  *lvpid   LVPID struct point
  * @retval None
  */
static void LVPID_Integral_Control(_LVPIDPARAM *lvpid)
{
	int IE_tmp,	IEaddPA;
	int Calc_tmp;

	if(lvpid->Ti == 0)  /*Ti == 0 : Disable Integral Control. Only has P or PD Control.*/
	{
		lvpid->IE = 0;
		lvpid->MV = lvpid->PA + lvpid->DA;
		
		InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);	
	}
	else   /*Ti<>0,  Integral Control  has effect.*/
	{
		/* ((Err + Err_Last)/2) * (Kp / Ti) *  dt_min */
		Calc_tmp = lvpid->Err + lvpid->Err_Last;
		Calc_tmp /= 2;
		Calc_tmp = Calc_tmp * lvpid->Kp;
		Calc_tmp /= lvpid->Ti;
		Calc_tmp *= lvpid->dt;
		
		/*Round */
		if((Calc_tmp < 59) && (Calc_tmp > 0)) 
			Calc_tmp = 60;
		if((Calc_tmp > -59) && (Calc_tmp < 0)) 
			Calc_tmp = -60;
		
		Calc_tmp /= 60;  /*dt s to min*/
		
		IE_tmp  = Calc_tmp + lvpid->IE_Last;

		/*initialize or Gain parameters don't changed.*/
		if((lvpid->flag.Bit.init == 1)||(lvpid->flag.Bit.newGain == 0))  
		{
			/*if this Integral Control is first time. use 0 as integral action.*/
			if(lvpid->flag.Bit.init == 1)  
			{
				IE_tmp = 0;
			}

			lvpid->MV = IE_tmp + lvpid->PA + lvpid->DA;	  /* I P D*/
			
		    InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);
		
			IEaddPA = IE_tmp + lvpid->PA;
			
			/*Integral Windup: also known as integrator windup or reset windup, 
			  refers to the situation in a PID feedback controller where a large change in setpoint occurs (say a positive change) 
			  and the integral terms accumulates a significant error during the rise (windup), 
			  thus overshooting and continuing to increase as this accumulated error is unwound (offset by errors in the other direction). 
			  The specific problem is the excess overshooting.*/
			if(!IsInRange(IEaddPA,MVRangeH,MVRangeL))
			{
				/*Preventing the integral term from accumulating above or below pre-determined bounds*/
				InRangeAndCoerce(IEaddPA, MVRangeH, MVRangeL);
				lvpid->IE = IEaddPA - lvpid->PA;				
			}
			else
			{
				lvpid->IE = IE_tmp;
			}
		}
		else  /*Gain parameters changed, but not initialize.*/
		{
			lvpid->MV = lvpid->MV_Last;
			lvpid->IE = lvpid->MV_Last - lvpid->PA - lvpid->DA;			
		}
	}
}





/**
  * @brief  Called in Timer IRQ. Update the Count_dt. 
  *         change flag :regulationOnce
  * @param  *lvpid   
  * @retval None
  */
void LVPID_UpdateSamplingPeriod_InIrq(_LVPIDPARAM *lvpid)
{
	if(lvpid->flag.Bit.enablePIDTask)
	{
		lvpid->Count_dt++;
		if(lvpid->Count_dt > lvpid->dtCount_Total)
		{
			lvpid->Count_dt = 0;
			lvpid->flag.Bit.regulationOnce = 1;
		}
	}
}



/**
  * @brief  Set PID gain
  * @param  *lvpid  LVPID struct point 
  * @param  kp:   LabVIEW KP * KpRATIO
  * @param  ti:   LabVIEW Ti * KpRATIO
  * @param  td:   LabVIEW Td * KpRATIO
  * @param  dt: second
  * @param  *lvpid  LVPID struct point 
  * @retval None
  */
void LVPID_SetGainAndDt(_LVPIDPARAM *lvpid, int kp, int ti, int td, int dt)
{
	lvpid->Kp = kp;
	lvpid->Ti = ti;
	lvpid->Td = td;
	lvpid->dt = dt;
	
	lvpid->Count_dt = 0;
	lvpid->dtCount_Total = dt * 1000 / UPDATE_SAMPLEPERIOD_IRQ_MS;
	lvpid->flag.Bit.newGain = 1;
	lvpid->flag.Bit.regulationOnce = 0;
}



/**
  * @brief  Reset PID or Initialize 
  * @param  *lvpid  LVPID struct point 
  * @retval None
  */
void LVPID_Init_or_Reset(_LVPIDPARAM *lvpid)
{
	lvpid->PV = *lvpid->pPV;
	lvpid->SV = *lvpid->pSV;
	lvpid->Err = lvpid->SV - lvpid->PV;
	
	LVPID_Proportional_Control(lvpid);
	
	lvpid->MV = lvpid->PA;
	InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);

	lvpid->IE = 0;
	lvpid->DA = 0;

 	lvpid->PV_Last = lvpid->PV;
 	lvpid->DA_Last = lvpid->DA;
	lvpid->Err_Last = lvpid->Err;
	lvpid->IE_Last = lvpid->IE;
	lvpid->MV_Last = lvpid->MV;
	
	lvpid->flag.Bit.init = 1;
}



/**
  * @brief  NI LabVIEW :    NI_PID_pid.lvlib:PID.vi
  * @param  *lvpid   LVPID struct point
  * @retval None
  */
int pid_Control(_LVPIDPARAM *lvpid)
{
	lvpid->PV = *lvpid->pPV;
	lvpid->SV = *lvpid->pSV;
	lvpid->Err =lvpid->SV - lvpid->PV;

	/* The next three funcs must be called sequentially.*/
	/*P*/
	LVPID_Proportional_Control(lvpid);

	/*D*/
	LVPID_Derivative_Control(lvpid);  

	/*I*/
	LVPID_Integral_Control(lvpid);

	lvpid->PV_Last = lvpid->PV;
	lvpid->MV_Last = lvpid->MV;
	lvpid->IE_Last = lvpid->IE;
	lvpid->DA_Last = lvpid->DA;
	lvpid->Err_Last = lvpid->Err;

	/*after once PID, the new Gain flag need to set zero.*/
	lvpid->flag.Bit.newGain = 0;
	lvpid->flag.Bit.init = 0;
	
	return lvpid->MV; 
}



/**
  * @brief  NI LabVIEW :    NI_PID_pid.lvlib:PID.vi
  * @param  *lvpid   LVPID struct point
  * @retval real output
  */
int LVPID_PID_Control(_LVPIDPARAM *lvpid)
{
	int output;
	if(!lvpid->flag.Bit.enablePIDTask)
		return lvpid->MV_Last;
	if(!lvpid->flag.Bit.regulationOnce)
		return lvpid->MV_Last;	
	
	lvpid->flag.Bit.regulationOnce = 0;
	
  	pid_Control(lvpid);
	InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);	
	output = lvpid->MV; 
	return lvpid->implementMV(output);
}




/**
  * @brief  
  * @param  *CmdStr: Command Word
  * @param  *Cmd: Receive Buf.
  * @retval ret
  */
int PID_Command_Func(char *CmdStr, char *Cmd)
{
	int isPIDcmd;
	char AnswerStr[50];
	int ret;
	int firstalign;
	_LVPIDPARAM *tmpPID;
	ret = ERR_PARAM;
	isPIDcmd = 0;
	
	firstalign = (Cmd[1] == '&') ? 1 : 0;
	
	if(strcmp(CmdStr, "PID_STOP") == 0)
	{
		/*LEN &1STOP_PID*/
		int val[1];
		ret = ERR_PARAM;
		if((ParseCmdParam(Cmd + firstalign, val, 1) == 1) && X_InRange(val[0], 0, 3))
		{
			LVPID[val[0]].flag.Bit.enablePIDTask = 0;
			LVPID[val[0]].implementMV(0);
			ret = PASS;			
		}
		isPIDcmd = 1;
	}
	else if(strcmp(CmdStr, "PID_START") == 0) 
	{
		/*LEN &1START_PID*/
		int val[1];
		ret = ERR_PARAM;
		if((ParseCmdParam(Cmd + firstalign, val, 1) == 1) && X_InRange(val[0], 0, 3))
		{
			LVPID_Init_or_Reset(&LVPID[val[0]]);
			LVPID[val[0]].flag.Bit.enablePIDTask = 1;
			LVPID[val[0]].flag.Bit.regulationOnce = 1; /*Regulate immediately*/
			ret = PASS;			
		}
		isPIDcmd = 1;
	}
	else if(strcmp(CmdStr, "SETPID") == 0)
	{
		int val[5];
		ret = ERR_PARAM;
		if((ParseCmdParam(Cmd + firstalign, val, 5) == 5) && X_InRange(val[0], 0, 3))
		{
			LVPID_SetGainAndDt(&LVPID[val[0]], val[1], val[2], val[3], val[4]);
			ret = PASS;	
		}
		isPIDcmd = 1;			
	}
	else if(strcmp(CmdStr, "GETPID") == 0)
	{
		/*Len &1GETPID 0*/
		if((Cmd[10] >= '0') && (Cmd[10] <= '3'))
		{
			tmpPID =  &LVPID[Cmd[10] - '0'];
//			sprintf(AnswerStr,"&%cOK %d %d %d %d %d %d %c\r", ModuleAdressChar, 
//					tmpPID->Kp, tmpPID->Ti, tmpPID->Td, 
//					tmpPID->dt, *(tmpPID->pSV), *(tmpPID->pPV), tmpPID->flag.Bit.enablePIDTask + '0');
		}
		else
		{
//			sprintf(AnswerStr,"&%cERR 3\r", ModuleAdressChar);				
		}
//		CANSendString(AnswerStr, ModuleAdress);	
		isPIDcmd = 1;
		return 1;
		
	}
	else if(strcmp(CmdStr,"SETMV") == 0)
	{
		int val[2];
		ret = ERR_PARAM;
		if((ParseCmdParam(Cmd+1, val, 2) == 2) && X_InRange(val[0], 0, 3))
		{
			LVPID[val[0]].implementMV(val[1]);
			ret = PASS;
		}
		isPIDcmd = 1;		
	}

	if(isPIDcmd == 1)
	{
//		ResponseCmdByCan(ret);
		return 1;
	}
	return  0;
}





