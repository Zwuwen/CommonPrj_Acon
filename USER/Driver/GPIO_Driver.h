#ifndef __GPIO_H
#define	__GPIO_H

#include "AIA_Utilities.h"
/**
 * @brief Turn off or Turn on  LED :
 */

#define LED1_RUN(a);		(a == 1)? GPIO_SetBits(GPIOB,GPIO_Pin_13) : GPIO_ResetBits(GPIOB,GPIO_Pin_13); 

/* Exported functions ------------------------------------------------------------*/
void LED_GPIO_Config(void);

#endif
