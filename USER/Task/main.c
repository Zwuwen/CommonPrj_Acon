/**
  ******************************************************************************
  * @file    main.c 
  * @author  AIA Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"

#include "CAN_Driver.h"
#include "IIC_Driver.h"
#include "AIA_PID.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"

#include "PWM_Driver.h"
#include "TemperatureTask.h"
#include "AIA_SyncData.h"
#include "AIA_Persistence.h"
#include "StepperMotorTask.h"
#include "StepperMotor.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void NVIC_Config(void);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */ 
 	/* System clocks configuration ---------------------------------------------*/
	NVIC_Config();
	
	//////Driver Initialize///////////
	IIC_Driver_Init();
	CAN_Driver_Init();
	//PWM_Init();
	
	//////AIA_Lib Initialize///////////
	DistributePersistencData();
	
	//////Module Initialize////////////	
	ModuleCore_Init(StepperMotorTask_CmdProcess);
	
	////////////StepperMotor///////////
	StepperMotor_Init();
	
	//////Can Filter///////////	
	CanFilterSignature[0] = ModuleCore.normalRecvSignature;	
	CanFilterSignature[1] = ModuleCore.boardcastRecvSignature;
	CAN_Filter_Config(CanFilterSignature, FILTER_FRAMEID_NUMBER);

	if (SysTick_Config(SystemCoreClock/1000)) /* 1ms */
	{ 
		/* Capture error */ 
		while (1);
	}
	
	__enable_irq();
	

  /* Infinite loop */
	while (1)
	{
//		LVPID_PID_Control(TOTAL_PID_NUMBER);

		AIA_Protocol2_Handle(&ModuleCore);
		
	}
}



/**
  * @brief  ÖÐ¶ÏÅäÖÃ
  * @param  None
  * @retval None
  */
void NVIC_Config(void)  
{  

#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);   
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);
#endif

	/* 1 bit for pre-emption priority, 3 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   
	
}





#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
