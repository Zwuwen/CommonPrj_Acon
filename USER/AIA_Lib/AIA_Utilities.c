/**
  ******************************************************************************
  * @file    AIA_PID.c
  * @author  Bowen.he
  * @version V2.0
  * @date    25-March-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  * Proportional-Integral-Derivative (PID) control
  * Implements a PID controller using a PID algorithm for simple PID applications
  * or high speed control applications that require an efficient algorithm. The PID 
  * algorithm features control output range limiting with integrator anti-windup 
  * and bumpless controller output for PID gain changes.
  *
  * A typical PID lib contains 2 files. There are one core file: AIA_PID.c,
  * one configuration file: AIA_PID.h
  * refer to : NI_PID_pid.lvlib:PID.vi
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */ 
  
#include "AIA_Utilities.h" 
 
 
uint8_t	const HEX_2_ASCII[] = {'0', '1', '2', '3','4', '5', '6', '7','8', '9', 'A', 'B','C', 'D', 'E', 'F'};


/**
  * @brief  将字符串转为int型   读取长度固定为4个字节
  * @param  *p ：待转换的字符串指针
  *         radix ： 可能值为 10 或者16， 代表10进制或16进制
  * @retval res 返回转换后的结果
  */
u16 atou16(char *p, char radix)
{
	u16 res = 0;
	u8 i = 4;
	while(i--)
	{
		if(radix == 10)
			res = res * radix + (*p++ - '0');
		else if(radix == 16)
		{
			if(*p >= '0' && *p <= '9')
				res = res * radix + (*p++ - '0');
			else res = res * radix + *p++ - 'A' + 10;
		}
	}	
	return res;
}





/**
  * @brief  将字符串转为u8型   读取长度固定为2个字节
  * @param  *p ：待转换的字符串指针
  *         radix ： 可能值为 10 或者16， 代表10进制或16进制
  * @retval res 返回转换后的结果
  */
u8 atou8(char *p, char radix)
{
	u8 res = 0;
	u8 i = 2;
	while(i--)
	{
		if(radix == 10)
			res = res * radix + (*p++ - '0');
		else if(radix == 16)
		{
			if(*p >= '0' && *p <= '9')
				res = res * radix + (*p++ - '0');
			else res = res * radix + *p++ - 'A' + 10;
		}
	}	
	return res;
}





/**
  * @brief  转int型为十六进制 ASCII 0xABCD = "ABCD"
  * @param  None
  * @retval None
  */
uint8_t* u16toa(uint8_t *dest, u16 num)
{
	uint8_t n;
	uint16_t temp_int;

	temp_int = num;

	n = (uint8_t)(temp_int & 0x0f);
	dest[3] = HEX_2_ASCII[n];
	temp_int >>= 4;
	n = (uint8_t)(temp_int & 0x0f);
	dest[2] = HEX_2_ASCII[n];
	temp_int >>= 4;
	n = (uint8_t)(temp_int & 0x0f);
	dest[1] = HEX_2_ASCII[n];
	temp_int >>= 4;
	n = (uint8_t)(temp_int & 0x0f);
	dest[0] = HEX_2_ASCII[n];

	return dest;
}





/**
  * @brief  转u8型为十六进制 ASCII 0xAB = "AB"
  * @param  None
  * @retval None
  */
uint8_t* u8toa(uint8_t *dest, u8 num)
{
	uint8_t n;
	uint8_t temp_u8;

	temp_u8 = num;

	n = temp_u8 & 0x0f;
	dest[1] = HEX_2_ASCII[n];
	temp_u8 >>= 4;
	n = temp_u8 & 0x0f;
	dest[0] = HEX_2_ASCII[n];

	return dest;
}



/**
  * @brief  求int型的绝对值
  * @param  x ：输入数据
  * @retval 返回x的绝对值
  */
int absint(int x)
{
	return ((x>0)? x : -x);
}



  
  
  
  
  
  
  
  
  
  






































