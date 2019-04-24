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


/**
  * @brief  
  * @param  
  * @retval res
  */
int AA_Process(AIAMODULE *module)/*relmove*/
{
	CHECK_RANGE_PARAM_1(0, 1);

	return PREPARE_IN_PROCESS;	
}









int StepperMotorTask_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(AA);	/*read temperature*/

		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}













