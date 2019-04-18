/**
  ******************************************************************************
  * @file    IIC_Driver.h
  * @author  Bowen.He
  * @version V1.1
  * @date    20-November-2015
  * @lastModify 15-April-2019
  * @brief   IIC Driver:  Each while statement has timeout.
  ******************************************************************************
  * @attention
  *
  * A typical IIC Driver contains 2 files. There are :
  *	iic_driver.c: six functions. I2C_Configuration should be called before other
  *			  	  functions were used.
  * iic_driver.h: In this file, user can modify some configuration.
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __IIC_DRIVER_H
#define	__IIC_DRIVER_H

#include "AIA_Utilities.h"


#define I2C_KEY 0x5a5a5a5c

#define I2C_Speed              10000 /*I2C Speed */
#define I2C2_SLAVE_ADDRESS7    0xA0  /*24C02 Address. Defined by hardware*/

#define I2CWAITCOUNT 30000


#define EEROM_ADDR_START_MOTOR 4
#define EEROM_ADDR_END_MOTOR 256

#define AT24C128		1
#define AT24C256		2

#define EEPROM_MODEL	AT24C256

#define EEPROM_Block0_ADDRESS 0xA0
#define EEPROM_Block1_ADDRESS 0xA2 
#define EEPROM_Block2_ADDRESS 0xA4 
#define EEPROM_Block3_ADDRESS 0xA6 

#define EEPROM_ADDRESS EEPROM_Block0_ADDRESS

#define I2C_PageSize           4  

#ifndef PARAM_SOURCE
#error "PARAM_SOURCE Not Defined!"
#elif PARAM_SOURCE == I2C_2

#define I2C_GPIO	(GPIOB)
#define I2C_SCL_PIN	(GPIO_Pin_10)
#define I2C_SDA_PIN	(GPIO_Pin_11)
#define I2C_CHANNEL	(I2C2)
#define I2C_RCC_APBPeriph_I2C	(RCC_APB1Periph_I2C2)
#define I2C_RCC_APBPeriph_GPIO	(RCC_APB2Periph_GPIOB)

#elif PARAM_SOURCE == I2C_1

#define I2C_GPIO	(GPIOB)
#define I2C_SCL_PIN	(GPIO_Pin_6)
#define I2C_SDA_PIN	(GPIO_Pin_7)
#define I2C_CHANNEL	(I2C1)
#define I2C_RCC_APBPeriph_I2C	(RCC_APB1Periph_I2C1)
#define I2C_RCC_APBPeriph_GPIO	(RCC_APB2Periph_GPIOB)

#else
#define I2C_GPIO	(GPIOB)
#define I2C_SCL_PIN	(GPIO_Pin_6)
#define I2C_SDA_PIN	(GPIO_Pin_7)
#define I2C_CHANNEL	(I2C1)
#define I2C_RCC_APBPeriph_I2C	(RCC_APB1Periph_I2C1)
#define I2C_RCC_APBPeriph_GPIO	(RCC_APB2Periph_GPIOB)
#endif




/* Exported functions ------------------------------------------------------------*/
void IIC_Driver_Init(void);
BOOL I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite);
BOOL I2C_EE_ByteWrite(u8* pBuffer, u16 WriteAddr);
BOOL I2C_EE_PageWrite(u8* pBuffer, u16 WriteAddr, u8 NumByteToWrite);
BOOL I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead);
BOOL I2C_EE_WaitEepromStandbyState(void);

BOOL SaveDataToI2cEprom(u8 *pData,u32 Address, u32 Length);
BOOL RestoreDataFromI2cEprom(u8 *pData,u32 Address, u32 Length);

#endif

