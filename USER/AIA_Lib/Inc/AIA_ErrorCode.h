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
#ifndef __AIA_ERRORCODE_H
#define	__AIA_ERRORCODE_H

/* Exported macro ------------------------------------------------------------*/

#define 	RESPONSE_IN_PROCESS			-1

#define 	PASS  						0
#define     FAIL                		1
#define 	ERR_CMDNOTIMPLEMENT			2
#define 	ERR_PARAM 					3
#define 	ERR_FIFO					4
#define 	ERR_ADDRESS					5
#define 	ERR_SEQUENCE				6







#endif
