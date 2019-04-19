/**
  ******************************************************************************
  * @file    AIA_Protocol2.0.c 
  * @author  Bowen.He
  * @version V2.0
  * @created    15-April-2019
  * @lastModify 
  * @brief   AIA_Protocol 2.0:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************
*/ 

#ifndef __AIA_PROTOCOL_H
#define	__AIA_PROTOCOL_H

#include "AIA_ModuleCore.h" 

#define MAXIMUM_SUPPORT_RECV_PARAMS 10
#define EXECUTE_SUCCESS 0


#define UPCASE2INT(A,B) ((A-'A')*26u + (B-'A'))
#define CASE_REGISTER_CMD_PROCESS(AB,A,B) 		case UPCASE2INT(A,B):\
												ret = AB##_Process(module);\
												break

//#define UPCASE2INT(A,B) ((int)(((#A)[0] - 'A')*26 + ((#B)[0] - 'A')))													
//#define CASE_REGISTER_CMD_PROCESS(A,B) 		case UPCASE2INT(A,B):\
//												ret = A##B##_Process(module);\
//												break
													
#define CHECK_PARAM_NUMBER(TOTAL)			if(module->validParams != TOTAL) return ERR_PARAM
#define CHECK_RANGE_PARAM(INDEX, PMIN, PMAX)  	if((module->validParams < INDEX) 		|| 	\
												   (module->recvParams[INDEX-1] < PMIN) ||	\
												   (module->recvParams[INDEX-1] > PMAX)) return ERR_PARAM

#define CHECK_RANGE_PARAM_1(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
#define CHECK_RANGE_PARAM_2(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
#define CHECK_RANGE_PARAM_3(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
#define CHECK_RANGE_PARAM_4(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
#define CHECK_RANGE_PARAM_5(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
#define CHECK_RANGE_PARAM_6(PMIN, PMAX) CHECK_RANGE_PARAM(1, PMIN, PMAX)
											
//	if(module->recvParams[0])													

/* Exported functions ------------------------------------------------------------*/
void AIA_Protocol2_Handle(AIAMODULE *module);
void PrepareResponseBuf(AIAMODULE *module, const char *fmt, ...);
void SendModuleResponse(AIAMODULE *module);												

#endif
