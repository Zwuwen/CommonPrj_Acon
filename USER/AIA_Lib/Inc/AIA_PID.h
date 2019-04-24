/**
  ******************************************************************************
  * @file    AIA_PID.h
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
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
#ifndef __AIA_PID_H
#define	__AIA_PID_H

#include "stm32f10x.h"

/* Exported macro ------------------------------------------------------------*/
#define TOTAL_PID_NUMBER 4

#define UPDATE_SAMPLEPERIOD_IRQ_MS 1	/*timer interrupt period 1ms to update Count_dt*/

#define MVRangeH 4000		/*MV max*/

#define MVRangeL 0			/*MV min*/

#define KpRATIO 100	/*Do not append an 'u' at the end.
					 Otherwise, every formula which contain KpRATIO will force cast to unsigned. */

typedef int (*IMPLEMENT_MV)(int val);

/** 
  * @brief  Proportional-Integral-Derivative (PID) control Varibale.
  * refer to : NI_PID_pid.lvlib:PID.vi
  */ 
typedef	struct
{
	int Kp;	 				/*P Proportional Gain (K) specifies the proportional gain of the controller.*/
	int Ti;  				/*I Integral time (Ti, min) specifies the integral time in minutes*/
	int Td;	 				/*D Derivative time (Td, min) specifies the derivative time in minutes.*/

	int dt;  				/*T dt (s) specifies the loop-cycle time, or interval in seconds, at which this PID func is called*/
	int PV;				    /*Process Variable*/
	int PV_Last;  			/*Last Process Variable*/
	int SV;  				/*SetPoint*/
	int MV;	 				/*Manipulated Variable : Output*/
	int MV_Last; 			/*Last Output*/

	int PA;					/*Proportional Action*/
	int PA_Last;			/*Last Proportional Action. (Not used during this algorithm)*/ 
	int IE;					/*Integral Error*/
	int IE_Last;			/*Last Integral Error*/  
	int DA;					/*Differential Action*/
	int DA_Last;			/*Last Differential Action*/ 

	int Err; 				/*error value e(t) as the difference between a desired setpoint (SP) and a measured process variable (PV). SV-PV*/
	int Err_Last; 			/*Last error value*/

	int *pPV;				/*point   Process Variable*/
	int *pSV;				/*point   SetPoint*/
	
	int dtCount_Total;		/*dt / UPDATE_SAMPLEPERIOD_IRQ_MS .*/
	int Count_dt;			/*compare with the dtCount_Total.*/
	
	IMPLEMENT_MV implementMV;
	
	/*Flags during process*/
	union{ 
		struct{
			unsigned enablePIDTask 	:1;	/*enable pid process*/
			unsigned regulationOnce	:1;
			unsigned newGain		:1;	/*new gain? As long as any parameter(Kp, Ti, Td) changes, the value need be TRUE.*/
			unsigned init			:1;	/*initialize or reset PID. After a PID regulation, this value should set to zero.*/

		}Bit;
		int Allbits;
	}flag;
}_LVPIDPARAM;

extern _LVPIDPARAM LVPID[TOTAL_PID_NUMBER];

/* Exported functions ------------------------------------------------------- */
void LVPID_Init_or_Reset(_LVPIDPARAM *lvpid);
void LVPID_UpdateSamplingPeriod_InIrq(_LVPIDPARAM *lvpid);
void LVPID_SetGainAndDt(_LVPIDPARAM *lvpid, int kp, int ti, int td, int dt);
int PID_Command_Func(char *CmdStr, char *Cmd);
								
int LVPID_PID_Control(u32 PidNum);

int implement_MV_Ch1(int val);
int implement_MV_Ch2(int val);
int implement_MV_Ch3(int val);
int implement_MV_Ch4(int val);
void LVPID_Variable_Init(void);

void LVPID_Param_FromPersistence(char *src);
int PerpareToPIDArea(char *dest);
								
#endif 

