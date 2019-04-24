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
#include "PWM_Driver.h"
#include "TemperatureTask.h"
#include "AIA_Persistence.h"

#include "stdio.h"
#include "string.h"

_LVPIDPARAM LVPID[TOTAL_PID_NUMBER];

int implement_MV_Ch1(int val)
{
	PWM_Ch1DutyChange(val); 
	return val;
}


int implement_MV_Ch2(int val)
{
	PWM_Ch2DutyChange(val); 
	return val;
}

int implement_MV_Ch3(int val)
{
	PWM_Ch3DutyChange(val); 
	return val;
}

int implement_MV_Ch4(int val)
{
	PWM_Ch4DutyChange(val); 
	return val;
}



void LVPID_Param_FromPersistence(char *src)
{
	int *tmp;
	int i;
	tmp = (int*)src;
	if(src != NULL)
	{
		for(i=0; i<TOTAL_PID_NUMBER; i++)
		{
			LVPID[i].Kp = *tmp++;
			LVPID[i].Ti = *tmp++;
			LVPID[i].Td = *tmp++;
			LVPID[i].dt = *tmp++;
			LVPID[i].flag.Bit.enablePIDTask = *tmp++;		
		}		
	}
	else
	{
		for(i=0; i<TOTAL_PID_NUMBER; i++)
		{
			LVPID[i].Kp = 100;
			LVPID[i].Ti = 0;
			LVPID[i].Td = 0;
			LVPID[i].dt = 1;
			LVPID[i].flag.Bit.enablePIDTask = 0;		
		}			
	}
}


int PerpareToPIDArea(char *dest)
{
	int *tmp;
	int i;
	tmp = (int*)dest;
	
	for(i=0; i<TOTAL_PID_NUMBER; i++)
	{
		*tmp++ = LVPID[i].Kp;
		*tmp++ = LVPID[i].Ti;
		*tmp++ = LVPID[i].Td;
		*tmp++ = LVPID[i].dt;
		*tmp++ = LVPID[i].flag.Bit.enablePIDTask;
	}
	
	return 5 * 4 * TOTAL_PID_NUMBER;
}




void LVPID_Variable_Init(void)
{
	int i;
	
	for(i=0;i<TOTAL_PID_NUMBER;i++)
	{
		LVPID[i].pSV = &SetTemp[i];
		LVPID[i].pPV = &ReadingTemp[i];	

		LVPID_SetGainAndDt(&LVPID[i], LVPID[i].Kp , LVPID[i].Ti ,LVPID[i].Td , LVPID[i].dt);	
		
		if(LVPID[i].flag.Bit.enablePIDTask == 1)
		{
			LVPID_Init_or_Reset(&LVPID[i]);
			LVPID[i].flag.Bit.regulationOnce = 1; /*Regulate immediately*/
			LVPID[i].Count_dt = 0;
		}
	}
	
	LVPID[0].implementMV = implement_MV_Ch1;
	LVPID[1].implementMV = implement_MV_Ch2;
	LVPID[2].implementMV = implement_MV_Ch3;
	LVPID[3].implementMV = implement_MV_Ch4;	
}


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



///**
//  * @brief  NI LabVIEW :    NI_PID_pid.lvlib:PID.vi
//  * @param  *lvpid   LVPID struct point
//  * @retval real output
//  */
//int LVPID_PID_Control(_LVPIDPARAM *lvpid, u32 PidNum)
//{	
//	int i;
//	int output;
//	
//	for(i=0;i<PidNum;i++)
//	{
//	
//	if(!lvpid->flag.Bit.enablePIDTask)
//		return lvpid->MV_Last;
//	if(!lvpid->flag.Bit.regulationOnce)
//		return lvpid->MV_Last;	
//	
//	lvpid->flag.Bit.regulationOnce = 0;
//	
//  	pid_Control(lvpid);
//	InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);	
//	output = lvpid->MV; 
//	return lvpid->implementMV(output);
//}
/**
  * @brief  NI LabVIEW :    NI_PID_pid.lvlib:PID.vi
  * @param  *lvpid   LVPID struct point
  * @retval real output
  */
int LVPID_PID_Control(u32 PidNum)
{	
	int i;
	int output;
	_LVPIDPARAM *lvpid;
	
	for(i=0;i<PidNum;i++)
	{
		lvpid = &LVPID[i];		
		if(!lvpid->flag.Bit.enablePIDTask)
			return lvpid->MV_Last;
		if(!lvpid->flag.Bit.regulationOnce)
			return lvpid->MV_Last;	
		
		lvpid->flag.Bit.regulationOnce = 0;
		
		pid_Control(lvpid);
		InRangeAndCoerce(lvpid->MV, MVRangeH, MVRangeL);	
		output = lvpid->MV; 
		lvpid->implementMV(output);
	}
	return PASS;
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int YA_Process(AIAMODULE *module) /* "PID_STOP"*/
{	
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	
	LVPID[module->recvParams[0]].flag.Bit.enablePIDTask = 0;
	LVPID[module->recvParams[0]].implementMV(0);		

	return PASS;
}


/**
  * @brief  
  * @param  
  * @retval res
  */
int YB_Process(AIAMODULE *module) /* "PID_START" */
{	
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	
	LVPID_Init_or_Reset(&LVPID[module->recvParams[0]]);
	LVPID[module->recvParams[0]].flag.Bit.enablePIDTask = 1;
	LVPID[module->recvParams[0]].flag.Bit.regulationOnce = 1; /*Regulate immediately*/

	return PASS;
}
/**
  * @brief  
  * @param  
  * @retval res
  */
int YC_Process(AIAMODULE *module) /* SETPID */
{
	CHECK_PARAM_NUMBER(5);
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	
	LVPID_SetGainAndDt(&LVPID[module->recvParams[0]], module->recvParams[1], module->recvParams[2], module->recvParams[3], module->recvParams[4]);	

	return PASS;
}
/**
  * @brief  
  * @param  
  * @retval res
  */
int YD_Process(AIAMODULE *module) /* "GETPID" */
{	
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	
	PrepareResponseBuf(module, "%d %d %d %d %d %d %d %c", EXECUTE_SUCCESS, 
		LVPID[module->recvParams[0]].Kp, LVPID[module->recvParams[0]].Ti, LVPID[module->recvParams[0]].Td, 
		LVPID[module->recvParams[0]].dt, *(LVPID[module->recvParams[0]].pSV), *(LVPID[module->recvParams[0]].pPV), LVPID[module->recvParams[0]].flag.Bit.enablePIDTask + '0');
		
	return PREPARE_IN_PROCESS;
}
/**
  * @brief  
  * @param  
  * @retval res
  */
int YE_Process(AIAMODULE *module) /* "SETMV" */
{
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);

	LVPID[module->recvParams[0]].implementMV(module->recvParams[1]);

	return PASS;
}

int PID_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(YA);	/* PID_STOP */
		CASE_REGISTER_CMD_PROCESS(YB);	/* PID_START */
		CASE_REGISTER_CMD_PROCESS(YC);	/* SETPID */
		CASE_REGISTER_CMD_PROCESS(YD);	/* "GETPID" */
		CASE_REGISTER_CMD_PROCESS(YE);	/* "SETMV" */
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}
