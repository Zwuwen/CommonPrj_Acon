/**
  ******************************************************************************
  * @file    AIA_ModuleCore.c
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"



AIAMODULE ModuleCore;

const char IdChar[] = { '0','1','2','3','4','5','6','7','8','9',
						'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
						'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	
/*-------Used here--------*/	
int SYNC_CmdProcess(AIAMODULE *module, int cmdword);
	

int ModuleCore_BroadcastCmdProcess(AIAMODULE *module, int cmdword);
int ModuleCore_NormalCmdProcess(AIAMODULE *module, int cmdword);	

	
	
/**
  * @brief  Initialize the CAN peripheral.
  * @param  moduleId: CoreModule Device id.
  * @retval None
  */
void ModuleCore_Init(void* userDefineFunc)
{
	strcpy(ModuleCore.Name, MODULE_NAME);
	ModuleCore.flag.Bit.init = 1;
	ModuleCore.address = 0x01;
	ModuleCore.addressChar = IdChar[ModuleCore.address];
	ModuleCore.normalRecvSignature = ModuleCore.address | 0x80;
	ModuleCore.boardcastIdChar = '0';
	ModuleCore.boardcastRecvSignature = 0x80;
	
	ModuleCore.sequence = '1';
	
	ModuleCore.BoardCastProcess = ModuleCore_BroadcastCmdProcess;
	ModuleCore.NormalProcess = ModuleCore_NormalCmdProcess;
	
	CanFilterSignature[0] = ModuleCore.normalRecvSignature;	
	CanFilterSignature[1] = ModuleCore.boardcastRecvSignature;
	
	InitCmdFIFO(&ModuleCore.fifo);
}


void ModuleCore_ModifyAddress(int address)
{
	ModuleCore.address = address;
	ModuleCore.addressChar = IdChar[ModuleCore.address];	
	ModuleCore.normalRecvSignature = ModuleCore.address | 0x80;
	
	__disable_irq();
	CAN_Driver_Init();
	
	CanFilterSignature[0] = ModuleCore.normalRecvSignature;	
	CanFilterSignature[1] = ModuleCore.boardcastRecvSignature;
	CAN_Filter_Config(CanFilterSignature, FILTER_FRAMEID_NUMBER);
	
	__enable_irq();
	
}





/**
  * @brief  RA Read Address.
  * @param  
  * @retval res
  */
int RA_Process(AIAMODULE *module)
{ 
	PrepareResponseBuf(module, "%d,%d", EXECUTE_SUCCESS, module->address);

	return PREPARE_IN_PROCESS;	
}



/**
  * @brief  
  * @param  
  * @retval res
  */
int RV_Process(AIAMODULE *module)
{
	if(module->validParams == 0)
	{
		module->recvParams[0] = 0;
	}
	
	/*validParams > 0*/
	switch(module->recvParams[0])
	{
		case 1:
			PrepareResponseBuf(module, "%d,%s,%s", EXECUTE_SUCCESS, __DATE__, __TIME__);
			break;
		case 0: 
		default:
			PrepareResponseBuf(module, "%d,%d,%d,%d", EXECUTE_SUCCESS, FIRST_VER, MIDDLE_VER, TEMP_VER);
			break;
	}
	
	return PREPARE_IN_PROCESS;
}




/**
  * @brief  
  * @param  
  * @retval res
  */
int SA_Process(AIAMODULE *module)
{
	CHECK_RANGE_PARAM_1(1, 62);
	
	PrepareResponseBuf(module, "%d", EXECUTE_SUCCESS);
	SendModuleResponse(module);

	ModuleCore_ModifyAddress(module->recvParams[0]);
	
	return RESPONSE_IN_PROCESS;
}



/**
  * @brief  
  * @param  
  * @retval res
  */
int ModuleCore_BroadcastCmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(RA, 'R', 'A');	/*Read address*/
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}	
	return ret;
}




/**
  * @brief  
  * @param  
  * @retval res
  */
int ModuleCore_NormalCmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	/*Reserved Cmd*/
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(RA, 'R', 'A');	/*Read address*/
		CASE_REGISTER_CMD_PROCESS(RV, 'R', 'V');	/*Read Version*/
		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'A');	/*Set Address*/
		CASE_REGISTER_CMD_PROCESS(SA, 'S', 'P');	/*Save Params*/
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}

	if(ret == ERR_CMDNOTIMPLEMENT)
	{
#if ENABLE_AIA_SYNC == 1
		ret = SYNC_CmdProcess(module, cmdword);
		if(ret != ERR_CMDNOTIMPLEMENT)
			return ret;		
#endif		
		
		
		
		if(module->UserDefineProcess != NULL)
		{
			ret = module->UserDefineProcess(module, cmdword);
		}	
	}
	
	return ret;
}





  
  
  
  
  
  
  
  
  






































