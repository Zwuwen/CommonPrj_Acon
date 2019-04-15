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
#include "AIA_ErrorCode.h"
#include "string.h"

AIAMODULE ModuleCore;

const char IdChar[] = { '0','1','2','3','4','5','6','7','8','9',
						'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
						'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};



/**
  * @brief  Initialize the CAN peripheral.
  * @param  moduleId: CoreModule Device id.
  * @retval None
  */
void ModuleCore_Init(void)
{
	strcpy(ModuleCore.Name, MODULE_NAME);
	ModuleCore.flag.Bit.init = 1;
}





int ModuleCore_HandleCmd(AIAMODULE *module)
{

	
	
	
	

	
	

}


  
  
  
  
  
  
  
  
  
  






































