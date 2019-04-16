#include "DemoTask.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"




int DemoTask_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	/*Reserved Cmd*/
	switch(cmdword)
	{
//		CASE_REGISTER_CMD_PROCESS(RA, 'R', 'A');	/*Read address*/
//		CASE_REGISTER_CMD_PROCESS(RV, 'R', 'V');	/*Read Version*/
//		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'A');	/*Set Address*/
//		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'P');	/*Save Params*/
		
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	if(ret == ERR_CMDNOTIMPLEMENT)
	{
		if(module->UserDefineProcess != NULL)
		{
			ret = module->UserDefineProcess(module, cmdword);
		}	
	}
	
	return ret;
}





































