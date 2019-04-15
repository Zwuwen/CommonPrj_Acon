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
#ifndef __AIA_UTILITIES_H
#define	__AIA_UTILITIES_H

#include "stm32f10x.h"


/* Exported macro ------------------------------------------------------------*/
#define FIRST_VER		(1)	
#define MIDDLE_VER		(0)
#define TEMP_VER		(0)

#define MODULE_NAME "AIA_Common_Module"

/* Exported Function Macro ------------------------------------------------------------*/
extern const char IdChar[];






/* Exported typedef ------------------------------------------------------------*/
typedef	struct
{
	int address;	/**/
	char addressChar;
	char Name[20];
	char responseBuf[100];
	
	/*Flags*/
	union{ 
		struct{
			unsigned init 			: 1;  	/*initialize.*/
		}Bit;
		int Allbits;
	}Flag;
}AIAMODULE;

extern AIAMODULE ModuleCore;



extern uint8_t	const HEX_2_ASCII[];

/* Exported functions ------------------------------------------------------- */





#endif
