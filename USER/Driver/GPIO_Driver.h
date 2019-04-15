/**
  ******************************************************************************
  * @file    GPIO_Driver.h
  * @author  AIA Team
  * @version V1.1
  * @created    15-April-2019
  * @lastModify 
  * @brief   GPIO Driver:  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __GPIO_DRIVER_H
#define	__GPIO_DRIVER_H

#include "AIA_Utilities.h"
/**
 * @brief Turn off or Turn on  LED :
 */

#define LED1_RUN(a);		(a == 1)? GPIO_SetBits(GPIOB,GPIO_Pin_13) : GPIO_ResetBits(GPIOB,GPIO_Pin_13); 

/* Exported functions ------------------------------------------------------------*/
void GPIO_Driver_Init(void);

#endif
