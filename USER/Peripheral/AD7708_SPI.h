
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AD7708_SPI_H
#define __AD7708_SPI_H

/* Includes ------------------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/

#define AD7708_PORT_RTC				RCC_APB2Periph_GPIOB

#define AD7708_RESET_PORT	    	GPIOB
#define AD7708_ReSet_PIN			GPIO_Pin_9

#define AD7708_SCLK_PORT			GPIOB
#define AD7708_SCLK_PIN			  	GPIO_Pin_13

#define AD7708_CS_PORT			 	GPIOB
#define AD7708_CS_PIN			    GPIO_Pin_12

#define AD7708_RDY_PORT			  	GPIOB
#define AD7708_RDY_PIN			 	GPIO_Pin_8

#define AD7708_DOUT_PORT			GPIOB
#define AD7708_DOUT_PIN			  	GPIO_Pin_14

#define AD7708_DIN_PORT				GPIOB
#define AD7708_DIN_PIN			  	GPIO_Pin_15

#define AD7708_DOUT_READ() GPIO_ReadInputDataBit(AD7708_DOUT_PORT,AD7708_DOUT_PIN)
#define AD7708_DELAY_CLK 2
#define AD7708_CLK_DELAY(x)  while(x--)

#define AD7708_CS(x)   	(x)?GPIO_SetBits(AD7708_CS_PORT,AD7708_CS_PIN): GPIO_ResetBits(AD7708_CS_PORT, AD7708_CS_PIN)
#define AD7708_SCLK(x)  (x)?GPIO_SetBits(AD7708_SCLK_PORT,AD7708_SCLK_PIN): GPIO_ResetBits(AD7708_SCLK_PORT, AD7708_SCLK_PIN)
#define AD7708_DOUT(x)  (x)?GPIO_SetBits(AD7708_DOUT_PORT,AD7708_DOUT_PIN): GPIO_ResetBits(AD7708_DOUT_PORT, AD7708_DOUT_PIN)
#define AD7708_DIN(x)   (x)?GPIO_SetBits(AD7708_DIN_PORT,AD7708_DIN_PIN): GPIO_ResetBits(AD7708_DIN_PORT, AD7708_DIN_PIN)


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

typedef struct
{
	u8 Register_Addr:4;
	u8 CR5_4:2;
	u8 RW:1;
	u8 WEN:1;
}REG_COMMUNICATION_T; 

typedef enum
{
	WEN_Enable 	= 0,
	WEN_Disable	= 1,
}WEN_E;

typedef enum
{
	Communications_WRITE		= 0,
	Communications_READ		=	1,
}READ_WRITE_E;

typedef enum
{
	Addr_Communications_Status	=	0,
	Addr_Mode					= 	1,
	ADDR_ADC_CONTROL			=	2,
	Addr_Filter					=	3,
	Addr_ADC_Data				=	4,  
	Addr_ADC_Offset				=	5,  
	Addr_ADC_Gain				=   6,  
	Addr_IO_Control				=   7,
	Addr_Test1					= 12, 
	Addr_Test2                 	= 13, 
	Addr_ID						= 15,
}REGISTER_ADDRESS_E;

typedef struct
{
	u8 Mode_Bits:3;
	u8 OSCPD:1;
	u8 CHCON:1;
	u8 REFSEL:1;
	u8 NEGBUF:1;
	u8 CHOP:1;
}REG_Mode_T;

typedef enum
{
	Mode_PowerDown					= 0, 
	Mode_Idle						= 1, 
	Mode_Single_Conversion 			= 2, 
	Mode_Continuous_Conversion		= 3, 
	Mode_InternalZeroCalibration	= 4, 
	Mode_InternalFullCalibration   	= 5, 
	Mode_SystemZeroCalibration		= 6, 
	Mode_SystemFullCalibration		= 7, 
}Mode_Choice;

typedef enum
{
	Oscillator_PowerON 		 = 0, 
	Oscillator_PowerOFF		 = 1, 
}Oscillator_Mode;

typedef enum
{
	CHCON_8InputConvert = 0,
	CHCON_10DifferInput	= 1,
}CHCON_Choice;

typedef enum
{
	REFSEL_1  = 0,
	REFSEL_2	= 1,
}REFSEL_Choice;

typedef enum
{
	NEGBUF_NoBuffer= 0, 
	NEGBUF_Buffer= 1,   
}NEGBUF_Choice;

typedef enum
{
	CHOP_Enable= 0,  
	CHOP_Disable= 1,
}CHOP_Choice;

typedef struct
{
	u8 ADC_Range:3;
	u8 Polar:1;
	u8 ADC_Channel:4;
}REG_ADC_Control_T;

typedef enum
{
	ADC_Range_20mV		=	0,
	ADC_Range_40mV		=	1,
	ADC_Range_80mV		=	2,
	ADC_Range_160mV		=	3,
	ADC_Range_320mV		=	4,
	ADC_Range_640mV		=	5,
	ADC_Range_1280mV	=	6,
	ADC_Range_2560mV	=	7,
}ADC_Range_t;

typedef enum
{
	BIPOLAR  = 0,
	UNIPOLAR = 1,
}Polar_t;

typedef enum
{
	P_AIN1_N_AINCOM 	=	0,
	P_AIN2_N_AINCOM 	=	1,
	P_AIN3_N_AINCOM 	=	2,
	P_AIN4_N_AINCOM 	=	3,
	P_AIN5_N_AINCOM 	=	4,
	P_AIN6_N_AINCOM 	=	5,
	P_AIN7_N_AINCOM 	=	6,
	P_AIN8_N_AINCOM 	=	7,
	P_AIN1_N_AIN2 		=	8,
	P_AIN3_N_AIN4 		=	9,
	P_AIN5_N_AIN6 		=	10,
	P_AIN7_N_AIN8 		=	11,
	P_AIN2_N_AIN2	 	=	12,
	P_AIN9_N_AIN10		=	12,
	P_AINCOM_N_AINCOM	=	13,
	P_REFP_N_REFN 		=	14,
	P_AIN9_N_AINCOM 	=	14,
	P_OPEN_N_OPEN 		=	15,
	P_AIN10_N_AINCOM 	=	15,
}ADC_Channel_t;

typedef struct
{
	u8 P1DAT:1;
	u8 P2DAT:1;
	u8 :2;
	u8 P1DIR:1;
	u8 P2DIR:1;
	u8 :2;
}REG_IO_Control_T;

typedef enum
{
	IOCON_IN	= 0,
	IOCON_OUT	= 1,
	
}IOCON_DIRECTION_E;

/**
  * @brief  Filter Register.
  * @param  ucSf: frequent of AD7708
  * @retval None
  */
typedef struct
{
	u8 ucSf;
	
}REG_FFILTER_T;

/* Exported variables ------------------------------------------------------- */

extern REG_ADC_Control_T    g_tAdcconReg;

/* Exported functions ------------------------------------------------------- */
extern void AD7708_BspInit(void);
extern int AD7708_RegisterWrite(REGISTER_ADDRESS_E reg,u16 registerData);
extern u16 AD7708_RegisterRead(REGISTER_ADDRESS_E reg);
extern int AD7708_Write8Bits(u8 inputData);
extern u8 AD7708_Read8Bits(u8 data);
extern float ReadChannleTemperature(u8);
extern void  AD7708_Init(void);
extern int AD7708_ChannelInternalCalibration(REG_ADC_Control_T *reg_ADCCON);
extern int AD7708_ChannelSystemCalibration(REG_ADC_Control_T *reg_ADCCON);
extern void OSTimeDly10Ms(uint16_t);
extern void AD7708_Config(void);
extern void GetTemperature(void);
extern void SPI_config(void);
extern void SPI_GPIO_Configuration(void);
extern u8 SPI_SendByte(u8 byte);
extern u8 SPI_ReadByte(u8 byte);
extern u8 SpiWriteData(u8 byte);
extern void AD7708_SPI2_Init(void);
extern void CANSend_SYNC(u16 value1, u16 value2, u16 value3, u16 value4, u32 id) ;

#endif
