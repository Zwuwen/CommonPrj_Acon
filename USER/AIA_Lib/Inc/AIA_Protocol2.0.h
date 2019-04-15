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


#define MAXIMUM_SUPPORT_RECV_PARAMS 10



#define UPCASE2INT(A,B) ((A-'A')*26u + (B-'A'))
#define CASE_REGISTER_CMD_PROCESS(AB,A,B) 		case UPCASE2INT(A,B):\
												ret = AB##_Process(module);\
												break
													
												
													

/* Exported functions ------------------------------------------------------------*/
int ParseCmdParam(char *cmd, int *val, int num);
void ResponseCmdByCan(int ret);

#endif
