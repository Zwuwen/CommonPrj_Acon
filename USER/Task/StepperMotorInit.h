/**
  ******************************************************************************
  * @file    StepperMotorInit.h
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

#define MOTOR_DIR_1		GPIO_Pin_0
#define MOTOR_STEP_1	GPIO_Pin_0
#define MOTOR_IN_1		GPIO_Pin_14
#define MOTOR_EN_1		GPIO_Pin_5
#define MOTOR_IN2_1		GPIO_Pin_3


#define ENCODER_PPR 	1024


#define GET_POSITIVESENSOR_STATE(pMOTOR) (pMOTOR->flag.Bit.EnableSensorLimitP ? \
										  pMOTOR->GetPositiveSensor() : FALSE)
#define GET_NEGATIVESENSOR_STATE(pMOTOR) (pMOTOR->flag.Bit.EnableSensorLimitN ? \
										  pMOTOR->GetNegativeSensor() : FALSE)
#define GET_ZSIGNAL_STATE(pMOTOR) 		  (pMOTOR->flag.Bit.EnableSensorLimitP ? \
										  pMOTOR->GetPositiveSensor() : FALSE)



/* Exported functions ------------------------------------------------------- */
void StepperMotor_Initialize_All(void);















