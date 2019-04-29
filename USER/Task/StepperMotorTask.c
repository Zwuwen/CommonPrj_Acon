/**
  ******************************************************************************
  * @file    StepperMotorTask.c 
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
#include "StepperMotorTask.h"
#include "StepperMotor.h"


/**
  * @brief  
  * @param  
  * @retval res
  */
int AA_Process(AIAMODULE *module)/*RST*/
{
	int ret;
	
	ret = MotorFindOriginBySensor(&StepperMotor[0]);
	
	return ret;	
}

int AB_Process(AIAMODULE *module)/*ABSMOVE*/
{
	CHECK_RANGE_PARAM_1(-500000, 500000);
	return MotorGotoA(&StepperMotor[0], PARAM_1, ABS_COORD);
}


int AC_Process(AIAMODULE *module)/*RELMOVE*/
{
	CHECK_RANGE_PARAM_1(-500000, 500000);
	return MotorGotoA(&StepperMotor[0], PARAM_1, REL_COORD);	
}






int StepperMotorTask_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(AA);	/*RST*/
		CASE_REGISTER_CMD_PROCESS(AB);	/*ABSMOVE*/
		CASE_REGISTER_CMD_PROCESS(AC);	/*RELMOVE*/
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}













