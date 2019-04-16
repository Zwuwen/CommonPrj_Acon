/**
  ******************************************************************************
  * @file    AIA_Persistence.c
  * @author  Bowen.he
  * @version V2.0
  * @created   	16-April-2019
  * @lastModify 16-April-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
  
#include "AIA_Persistence.h" 
#include "IIC_Driver.h"
#include "Flash_Driver.h"

PERSISTENCE_PARAM PersistenceParams;



BOOL PLL_SaveParams(char *pbuf, int length)
{
#ifndef PARAM_SOURCE
#error "PARAM_SOURCE Not Defined!"
#elif (PARAM_SOURCE == I2C_2) || (PARAM_SOURCE == I2C_1)


	
	

#elif PARAM_SOURCE == FLASH_3


	
	
#endif
	return TRUE;
}





BOOL PLL_ReadParams(char *pbuf, int length)
{
#ifndef PARAM_SOURCE
#error "PARAM_SOURCE Not Defined!"
#elif (PARAM_SOURCE == I2C_2) || (PARAM_SOURCE == I2C_1)


	
	

#elif PARAM_SOURCE == FLASH_3


	
	
#endif
	return TRUE;
}

























