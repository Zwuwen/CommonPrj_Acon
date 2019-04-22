#include "TemperatureTask.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"
#include "AIA_Persistence.h"


///**
//  * @brief  
//  * @param  
//  * @retval res
//  */
//int FT_Process(AIAMODULE *module)
//{
//	int i;
//	for(i= 0;i<5*1024;i++)
//	{
//		PersistenceParams.moduleId[i] = 3;
//	}
//	PLL_SaveParams((char*)&PersistenceParams,sizeof(PersistenceParams));
//	for(i= 0;i<5*1024;i++)
//	{
//		PersistenceParams.moduleId[i] = 4;
//	}
//	
//	PLL_ReadParams((char*)&PersistenceParams,sizeof(PersistenceParams));
//	
//	PrepareResponseBuf(module, "%d,%d", EXECUTE_SUCCESS, module->address);

//	return PREPARE_IN_PROCESS;	
//}

int TemperatureTask_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	/*Reserved Cmd*/
	switch(cmdword)
	{
//		CASE_REGISTER_CMD_PROCESS(FT);	/*Save Params*/
////		CASE_REGISTER_CMD_PROCESS(RA, 'R', 'A');	/*Read address*/
////		CASE_REGISTER_CMD_PROCESS(RV, 'R', 'V');	/*Read Version*/
////		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'A');	/*Set Address*/
////		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'P');	/*Save Params*/
		
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}





































