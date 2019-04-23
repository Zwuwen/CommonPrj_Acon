/********************************************************************************
  * @file    ADC_Driver.h 
  * @author  AIA Team
  * @version V1.1
  * @created    19-April-2019
  * @lastModify 
  * @brief   ADC Driver:  
  ******************************************************************************/

#ifndef __PWM_DRIVER_H
#define	__PWM_DRIVER_H

#include "stm32f10x.h"

extern void PWM_Init(void);
extern u8 Ch1_Duty_Change(u16 Duty_Ratio);
extern u8 Ch2_Duty_Change(u16 Duty_Ratio);

extern u8 PWM_Ch1DutyChange(u16 Duty_Ratio);
extern u8 PWM_Ch2DutyChange(u16 Duty_Ratio);
extern u8 PWM_Ch3DutyChange(u16 Duty_Ratio);
extern u8 PWM_Ch4DutyChange(u16 Duty_Ratio);

extern u8 PWM_Change(u32 _Frequency, u16 _DutyRatio, u8 _ucChannel);

#endif
