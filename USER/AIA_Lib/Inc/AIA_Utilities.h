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

/*Exported Config*/
#define PARAM_SOURCE I2C_1

/* Exported macro ------------------------------------------------------------*/
#define LOW  0
#define HIGH 1

#define I2C_1	1
#define I2C_2	2
#define FLASH_3	3


/* Exported Function Macro ------------------------------------------------------------*/
#define X_InRange(X, MIN, MAX) ((X>=MIN)&&(X<=MAX))

#define InRangeAndCoerce(x,H,L) if(x > H) x = H;\
								else if(x < L) x = L
								
#define IsInRange(x, H, L) ((x<=H) && (x>=L))

								
#define DELAY_10MS(TIMEMS) Delay_Count = TIMEMS; while(Delay_Count!=0)


/* Exported typedef ------------------------------------------------------------*/
/** 
  * @brief  bool
  */ 
typedef enum  
{
	FALSE = 0, 
	TRUE
}BOOL;


/** 
  * @brief  function return state
  */
typedef enum  
{
	RET_SUCCESS = 0, 
	RET_FAIL = 1
}RETSTAT;


/** 
  * @brief  Global Flag bit define  
  */ 
typedef union
{
	struct
	{
		unsigned SyncTask				:1; 
		unsigned Bit2					:1;
		unsigned Bit3					:1; 
		unsigned LedRunStatus			:1;
		unsigned LedAlermStatus			:1; 
		unsigned Bit5					:1;
		unsigned Bit6					:1; 
		unsigned Bit7					:1;

		unsigned delay100ms				:1; 
		unsigned Bit9					:1;
		unsigned Bit10					:1; 
		unsigned Bit11					:1;
		unsigned Bit12					:1; 
		unsigned Bit13					:1;
		unsigned AD7708_Delay			:1; 
		unsigned Terminate				:1;
	}Bit;
	uint32_t Allbits;
}FLAG;
extern FLAG Flag;


extern uint8_t	const HEX_2_ASCII[];



/* Exported functions ------------------------------------------------------- */
u16 atou16(char *p, char radix);
u8 atou8(char *p, char radix);
uint8_t* u16toa(uint8_t *dest, u16 num);
uint8_t* u8toa(uint8_t *dest, u8 num);
int absint(int x);


#endif
