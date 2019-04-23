/**
  ******************************************************************************
  * @file    AD7708_SPI.c 
  * @author  Xiufeng.Zhu
  * @version V1.0
  * @created    20-April-2019
  * @lastModified 20-April-2019
  * @brief   IC AD7708  via SPI   8 ADC value
  ******************************************************************************
  * @attention
  *
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/

#include "AD7708_SPI.h"
#include <string.h>
#include <math.h>



/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* AD7708 register */
REG_COMMUNICATION_T  		g_tCommReg;
REG_IO_Control_T			g_tIoconReg;
REG_FFILTER_T				g_tFilterReg;
REG_ADC_Control_T     		g_tAdcconReg;
REG_Mode_T					g_tModeReg;

/* AD7708 channel */
ADC_Channel_t g_aAdcChannel[10]={		
		P_AIN1_N_AINCOM,P_AIN2_N_AINCOM,
		P_AIN3_N_AINCOM,P_AIN4_N_AINCOM,
		P_AIN5_N_AINCOM,P_AIN6_N_AINCOM,
		P_AIN7_N_AINCOM,P_AIN8_N_AINCOM,
		P_AIN9_N_AINCOM,P_AIN10_N_AINCOM
};

u8 g_IsLongData[16] = {0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0};/* 16bit or 8bit of regiser  */
u8 g_Gain[8]= {128,84,32,16,8,4,2,1};	/* gain */
double g_ValTest[10];
u8 g_ucLastChannel = 16;/* save last ad7708 channel */


/**
  * @brief  short delay use clk  .
  * @param  clk*x?
  * @retval None
  */
void AD7708_ClkDelay(u16 _ustime)
{
    u16 i;

    for(i=0;i<_ustime;i++){}
}

#if 0

/**
  * @brief  init spi interfce of io  Io simulation  .
  * @param  None
  * @retval None
  */
void AD7708_Hard_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	RCC_APB2PeriphClockCmd(AD7708_PORT_RTC, ENABLE);

  	/* SCLK */
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		=	AD7708_SCLK_PIN;
	GPIO_Init(AD7708_SCLK_PORT,&GPIO_InitStructure);
	/* CS */
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		=	AD7708_CS_PIN;
	GPIO_Init(AD7708_CS_PORT,&GPIO_InitStructure);
	AD7708_CS(0);
  	/* DIN */
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin		=	AD7708_DIN_PIN;
	GPIO_Init(AD7708_DIN_PORT,&GPIO_InitStructure);
	/* DOUT */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
	GPIO_InitStructure.GPIO_Pin = AD7708_DOUT_PIN;		
	GPIO_Init(AD7708_DOUT_PORT, &GPIO_InitStructure);	
}


/**
  * @brief  write 8bit to AD7708  by io simulation spi
  * @param  None
  * @retval None
  */
int  AD7708_Write8Bits(u8 _ucInputData)
{
	u8 i, ucDelayClk;

	ucDelayClk = AD7708_DELAY_CLK;
	AD7708_CLK_DELAY(ucDelayClk);
	
	for(i=0; i<8; i++)
	{
		AD7708_SCLK(0);
		AD7708_CLK_DELAY(ucDelayClk);
		
		((_ucInputData) & 0x80) ? AD7708_DIN(1): AD7708_DIN(0);
		_ucInputData <<= 1;
		
		AD7708_CLK_DELAY(ucDelayClk);
		AD7708_SCLK(1);
		AD7708_CLK_DELAY(ucDelayClk);
	}
	return 0;
}

/**
  * @brief  read 8bit from AD7708  .
  * @param  None
  * @retval None
  */
u8 AD7708_Read8Bits(u8 _ucdata)
{
	u8 i,ucOutputData,ucDelayClk;

	ucOutputData = 0;
	ucDelayClk = AD7708_DELAY_CLK;
	
	AD7708_CLK_DELAY(ucDelayClk);

	for(i=0; i<8; i++)
	{
		AD7708_SCLK(0);
		AD7708_CLK_DELAY(ucDelayClk);
		AD7708_DIN(1);							
		AD7708_SCLK(1);
		AD7708_CLK_DELAY(ucDelayClk);
		ucOutputData = (ucOutputData << 1) | AD7708_DOUT_READ();/*read spi _ucdata-MSB input first*/
	}
	return ucOutputData;
}
#endif
/**
  * @brief  write data to AD7708 register  .
  * @param  None
  * @retval None
  */
int AD7708_RegisterWrite(REGISTER_ADDRESS_E _RegisterAddr,u16 _RegisterData)
{
	u8 ucTempdata8bits,ucDataL,ucDataH;
	u16 usData;

	usData = _RegisterData;
	ucTempdata8bits = 0;
		
	g_tCommReg.RW				=   Communications_WRITE;  	/* write enable */
	g_tCommReg.WEN			=	WEN_Enable;			  	/* communicatoin enalbe */
	g_tCommReg.CR5_4			= 	0;
	g_tCommReg.Register_Addr	=	_RegisterAddr;					
	memcpy(&ucTempdata8bits,&g_tCommReg,sizeof(ucTempdata8bits));
	
 	SPI_SendByte(ucTempdata8bits);
	if(g_IsLongData[_RegisterAddr]) /* if register is 16 bit */
	{
		ucDataH = (u8)((usData&0xFF00)>>8);
		ucDataL = (u8)(usData&0x00FF);
		SPI_SendByte(ucDataH);
		SPI_SendByte(ucDataL);
	}
	else
	{
		SPI_SendByte((u8)(usData&0x00FF));
	}
	return 0;
}

/**
  * @brief  read data from AD7708 register  .
  * @param  None
  * @retval None
  */
u16 AD7708_RegisterRead(REGISTER_ADDRESS_E _RegisterAddr)
{
	u8 ucTempdata,ucDataH,ucDataL;

	ucTempdata = ucDataH = ucDataL = 0;
	
	g_tCommReg.RW						=   Communications_READ;  		/*read enable*/
	g_tCommReg.WEN					=	WEN_Enable;				/* communication enable */
	g_tCommReg.CR5_4					= 	0;
	g_tCommReg.Register_Addr			=	_RegisterAddr;	 		/* channel */
	memcpy(&ucTempdata,&g_tCommReg,sizeof(ucTempdata));

	SPI_SendByte(ucTempdata);
	if(g_IsLongData[_RegisterAddr]) ucDataH = SPI_SendByte(0xFF);				/* if register is 16 bit */
		
	ucDataL= SPI_SendByte(0xFF);
	return ((u16)ucDataH<<8)|ucDataL;
}


/**
  * @brief  check ready of status register  .
  * @param  None
  * @retval None
  */
u16 AD7708_CheckReady(void)
{
	u16 usStatus;
	u32 ulRetryCnt;

	ulRetryCnt  = 0;
	
	usStatus = AD7708_RegisterRead(Addr_Communications_Status);
	while((usStatus&0x80)==0)
	{
		AD7708_ClkDelay(5);
		usStatus = AD7708_RegisterRead(Addr_Communications_Status);
		if(++ulRetryCnt>10000)
		{
			return 1;
		}
	}
	return 0;
}

/**
  * @brief  AD7708 Init  .
  * @param  None
  * @retval None
  */
void AD7708_Init()
{
	u8 i;
	u8 ucTempData;

	memset(&g_tCommReg,0,sizeof(g_tCommReg));
	memset(&g_tIoconReg,0,sizeof(g_tIoconReg));
	memset(&g_tFilterReg,0,sizeof(g_tFilterReg));
	memset(&g_tAdcconReg,0,sizeof(g_tAdcconReg));
	memset(&g_tModeReg,0,sizeof(g_tModeReg));
	
	/*soft reset*/
	g_tModeReg.CHOP  		= CHOP_Enable;       /*enable chop for stable result (slow speed)*/
	g_tModeReg.NEGBUF		= NEGBUF_NoBuffer; 	 /* no buffer */
	g_tModeReg.REFSEL		= REFSEL_1;         
	g_tModeReg.CHCON 		= CHCON_10DifferInput;
	g_tModeReg.OSCPD 		= Oscillator_PowerON; 
	g_tModeReg.Mode_Bits = Mode_PowerDown;	/* soft reset */
	memcpy(&ucTempData,&g_tModeReg,sizeof(ucTempData));
	AD7708_RegisterWrite(Addr_Mode,(uint16_t)ucTempData);
	
	/* config AD7708 IO Control register */
	g_tIoconReg.P1DIR = IOCON_IN; /*P1 IO input mode */
	g_tIoconReg.P2DAT = IOCON_IN; /*P2 IO input mode */
	memcpy(&ucTempData,&g_tIoconReg,sizeof(ucTempData));
	AD7708_RegisterWrite(Addr_IO_Control,(uint16_t)ucTempData);
	
	/* config AD7708 filter register */
	g_tFilterReg.ucSf = 0x10;			
	AD7708_RegisterWrite(Addr_Filter,(uint16_t)g_tFilterReg.ucSf);					
									
	for(i=0;i<10;i++)
	{
		g_tAdcconReg.ADC_Channel=g_aAdcChannel[0];
		g_tAdcconReg.ADC_Range		= ADC_Range_2560mV;  		/* Selected ADC Input Range +/- 2.56V*/
		g_tAdcconReg.Polar				= UNIPOLAR;          	/*unipolar*/
		memcpy(&ucTempData,&g_tAdcconReg,sizeof(ucTempData));
		AD7708_RegisterWrite(ADDR_ADC_CONTROL,(uint16_t)ucTempData);	
		AD7708_ChannelInternalCalibration(&g_tAdcconReg);	/* calibration */
	}
	/* config AD7708 mode register */
	g_tModeReg.CHOP  		= CHOP_Enable;      /*enable chop for stable result (slow speed)*/
	g_tModeReg.NEGBUF		= NEGBUF_NoBuffer;  /* no buffer */
	g_tModeReg.REFSEL		= REFSEL_1;         
	g_tModeReg.CHCON 		= CHCON_10DifferInput;
	g_tModeReg.OSCPD 		= Oscillator_PowerON; 
	g_tModeReg.Mode_Bits = Mode_Continuous_Conversion;/* continuous read mode */
	memcpy(&ucTempData,&g_tModeReg,sizeof(ucTempData));
	AD7708_RegisterWrite(Addr_Mode,(uint16_t)ucTempData);
}

/**
  * @brief  calculate temperature formula  .
  * @param  None
  * @retval None
  */
float CalTemperature(float _fRt)
{
	float fBx,fKa,fRt,fTemp,fRp,fT2;
	
	fBx = 3950.0;
	fKa = 273.15;
	fRp = 10000.0; 
	fT2 = (273.15+25.0);
	
	fRt = _fRt;
						/*like this R=5000, fT2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25))*/
	fTemp = fRt/fRp;
	fTemp = log(fTemp);	/*ln(fRt/fRp)*/
	fTemp/=fBx;			/*ln(fRt/fRp)/B*/
	fTemp+=(1/fT2);
	fTemp = 1/(fTemp);
	fTemp-=fKa;
	
	return fTemp;
}
/**
  * @brief  read adc voltage  .
  * @param  None
  * @retval None
  */
float ReadChannleTemperature(u8 _ucAdcChannel)
{
	u32 ulTempData,ulReadData;
	float fResistance,fVoltage;

	ulReadData = 0;
	
	if(_ucAdcChannel != g_ucLastChannel)
	{
		/*config channel */
		g_ucLastChannel = _ucAdcChannel ;
		g_tAdcconReg.ADC_Channel= _ucAdcChannel;
		memcpy(&ulTempData,&g_tAdcconReg,sizeof(ulTempData));
		AD7708_RegisterWrite(ADDR_ADC_CONTROL,(uint16_t)ulTempData);
	}
	
	if(AD7708_CheckReady())
	{
		return 1;
	}
	/* directly date form adc */
	ulReadData = AD7708_RegisterRead(Addr_ADC_Data);
	
	if(UNIPOLAR == g_tAdcconReg.Polar) 
	{	
		/* voltage valuble */
		fVoltage = (1.024*(ulReadData*3300)/(65536*g_Gain[g_tAdcconReg.ADC_Range])); /*Voltage(mV)= ADC_Dat*1.024*Vref(mv)/[(2xy16)*g_Gain]*/

		if(P_AIN9_N_AINCOM == g_tAdcconReg.ADC_Channel) /* channel 9 return ref voltage */
		{		
			return 6*fVoltage;
		}
		else if(P_AIN10_N_AINCOM == g_tAdcconReg.ADC_Channel) /* channel 10 return resistance */
		{
			/* resistance  */
			fResistance = (12000*((ulReadData)/(65536*g_Gain[g_tAdcconReg.ADC_Range]))-2000)/
						  (1-((ulReadData)/(65536*g_Gain[g_tAdcconReg.ADC_Range])));		
			if(fResistance<=0) fResistance = 1;		
			return CalTemperature(fResistance);
		}
		else
		{
			/* resistance  */
			fResistance = (12000*((ulReadData)/(65536*g_Gain[g_tAdcconReg.ADC_Range]))-2000)/
						  (1-((ulReadData)/(65536*g_Gain[g_tAdcconReg.ADC_Range])));	
			return 100*CalTemperature(fResistance);/* returm temperature */
		}
	}
	else if(BIPOLAR == g_tAdcconReg.Polar)
	{
		fVoltage = (((ulReadData/32768)-1)*3300)/g_Gain[g_tAdcconReg.ADC_Range];/*Voltage(mV) = (ADC_Dat/2xy15-1)*(1.024*Vref(mv))/AIN*/
		/* resistance  */
		fResistance = (12000*((fVoltage)/(65536*g_Gain[g_tAdcconReg.ADC_Range]))-2000)/
					  (1-((fVoltage)/(65536*g_Gain[g_tAdcconReg.ADC_Range])));	
		return CalTemperature(fResistance);/* returm temperature */
	}
	return 1;
}
/**
  * @brief  self calibration  .
  * @param  None
  * @retval None
  */
int AD7708_ChannelInternalCalibration(REG_ADC_Control_T *pRegAdccon)
{
	u32 ulTempData;
	u32 ulRetryCnt;
	
	ulRetryCnt=0;
	/*self-calibration zero point */
	g_tModeReg.CHCON				=		CHCON_10DifferInput;
	g_tModeReg.CHOP					=		CHOP_Enable;
	g_tModeReg.Mode_Bits			=	  	Mode_InternalZeroCalibration;
	g_tModeReg.NEGBUF				=  		NEGBUF_NoBuffer;
	g_tModeReg.OSCPD				=   	Oscillator_PowerON;
	g_tModeReg.REFSEL				=		REFSEL_1;
	memcpy(&ulTempData,&g_tModeReg,sizeof(ulTempData));
	AD7708_RegisterWrite(Addr_Mode,(u16)ulTempData);

	/*wait no busy */
	while((AD7708_RegisterRead(Addr_Mode)&0x07)!=0x01)
	{
		AD7708_ClkDelay(5);
		if(++ulRetryCnt==100000)
		{
			return 1;
		}
	}
	/*self-calibration full range */
  	g_tModeReg.Mode_Bits		= 	Mode_InternalFullCalibration;
	memcpy(&ulTempData,&g_tModeReg,sizeof(ulTempData));

	/*wait no busy */
	while((AD7708_RegisterRead(Addr_Mode)&0x07)!=0x01)
	{
		AD7708_ClkDelay(5);
		if(++ulRetryCnt==100000)
		{
			return 1;
		}
	}
	return 0;
}

/**
  * @brief  system calibration  .
  * @param  None
  * @retval None
  */
int  AD7708_ChannelSystemCalibration(REG_ADC_Control_T *pRegAdccon)
{
	u32 ulTempData;

	/* channel for calibration */
	memcpy(&ulTempData,pRegAdccon,sizeof(ulTempData));
	AD7708_RegisterWrite(ADDR_ADC_CONTROL,(u16)ulTempData);
	/* self calibration zero point */
	g_tModeReg.CHCON			=	CHCON_8InputConvert;
	g_tModeReg.CHOP				=	CHOP_Enable;
	g_tModeReg.Mode_Bits		=	Mode_SystemZeroCalibration;
	g_tModeReg.NEGBUF			=  	NEGBUF_NoBuffer;
	g_tModeReg.OSCPD			=   Oscillator_PowerON;
	g_tModeReg.REFSEL			=	REFSEL_1;
	memcpy(&ulTempData,&g_tModeReg,sizeof(ulTempData));
	AD7708_RegisterWrite(Addr_Mode,(u16)ulTempData);

	/* wait no busy */
	if(AD7708_CheckReady())
	{
		return 1;
	}

	return 0;
}

/**
  * @brief  AD7708 config  .
  * @param  None
  * @retval None
  */
void AD7708_Config(void)
{
	AD7708_SPI2_Init();
	AD7708_Init();
}

/**
  * @brief  SPI2 Init .
  * @param  None
  * @retval None
  */
void AD7708_SPI2_Init(void) 
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* DIN DOUT SCLK */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|AD7708_PORT_RTC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = AD7708_SCLK_PIN |AD7708_DIN_PIN| AD7708_DOUT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(AD7708_CS_PORT, &GPIO_InitStructure);

	/* CS*/
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin 	=	AD7708_CS_PIN;
	GPIO_Init(AD7708_CS_PORT,&GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE); 
	
	AD7708_CS(0);/* enable ad7708 */
}

/**
  * @brief  SPI2 send one byte .
  * @param  byte to send
  * @retval one byte received when send on byte
  */
u8 SPI_SendByte(u8 _ucByte)
{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
	
	SPI_I2S_SendData(SPI2, _ucByte);
	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);/*  or while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} */
	
	return SPI_I2S_ReceiveData(SPI2);
}

/**
  * @}
  */

