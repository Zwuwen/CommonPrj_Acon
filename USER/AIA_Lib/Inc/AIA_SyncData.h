/**
  ******************************************************************************
  * @file    AIA_SyncData.h
  * @author  Bowen.he
  * @version V2.0
  * @date    27-March-2019
  * @brief   AIA_Library  
  ******************************************************************************
  * @attention
  * This lib is used for real-time data transmit. 
  * CAN standard frame id has 11bit. AIA device use the low 8bit, high 3bit is useless
  * for the AIA_Procotol.
  * Use (DeviceID | 0x700) as can frameID, for hexadecimal data transmission.
  * can data area have 8 byte: first byte is total package number. second byte is package index.
  * The remaining six bytes are effective data. 
  * For example: package 1 :0x02 0x01 0x01 0x02 0x03 0x04 0x05 0x06
  * 			 package 2 :0x02 0x02 0x07 0x08 0x09 0x0a 0x0b 0x0c
  * transmit the valid data: 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
  * SYNCDATA_FORMAT:  '1': byte;	'2': i16;	'3': invalid;	'4':int
  * A typical SyncData lib contains 2 files. There are one core file: AIA_SyncData.c,
  * one configuration file: AIA_SyncData.h
  * Use with AIASyncData.exe
  * 
  * <h2><center>&copy; COPYRIGHT 2019 ACONBIO</center></h2>
  ******************************************************************************
  */  
#ifndef __AIA_SYNCDATA_H
#define	__AIA_SYNCDATA_H

#include "stm32f10x.h"

/* Exported macro ------------------------------------------------------------*/
#define cast(t, exp)	((t)(exp))	/*cast*/

#define CAST_pVOID(i)	 cast(void*, (i))  
#define CAST_pINT(i)	 cast(int*, (i))/*pointer*/
#define CAST_pS16(i)	 cast(s16*, (i))
#define CAST_pCHAR(i)	 cast(char*, (i))


#define UPDATE_SYNCDATAFLAG_IRQ_MS 10	/*timer interrupt period 10ms to update Count_dt*/
#define DEFAULT_SYNCDATA_UPLOAD_PERIOD 100	/*100ms*/

////////Modify here/////////////
#define LOAD_SYNCDATA(A,B,C,D,E,F,G,H)	*CAST_pS16(SyncBuffer 	+ 0) = A;\
										*CAST_pS16(SyncBuffer 	+ 2) = B;\
									    *CAST_pS16(SyncBuffer 	+ 4) = C;\
										*CAST_pS16(SyncBuffer 	+ 6) = D;\
										*CAST_pS16(SyncBuffer 	+ 8) = E;\
										*CAST_pS16(SyncBuffer 	+ 10) = F;\
										*CAST_pS16(SyncBuffer 	+ 12) = G;\
										*CAST_pS16(SyncBuffer 	+ 14) = H\

/*'1': byte;	'2': i16;	'3': invalid;	'4':int*/
#define SYNCDATA_FORMAT "22222222"

extern char SyncBuffer[100];

/** 
  * @brief  SYNC data
  */ 
typedef	struct
{
	int validByteNumber;	/*upload valid bytes*/
	int periodCount_Total;	/*uploadPeriod / UPDATE_SYNCDATAFLAG_IRQ_MS .*/
	int periodCount_i;		/*compare with the periodCount_Total.*/
	int uploadPeriod;		/**/

	/*Flags*/
	union{ 
		struct{
			unsigned syncTask 		: 1;	/*enable upload sync*/
			unsigned uploadOnce 	: 1;	/*upload once*/
			unsigned init 			: 1;  	/*initialize.*/
		}Bit;
		int Allbits;
	}flag;
}_SYNCDATA;

extern _SYNCDATA SyncData;


/* Exported functions ------------------------------------------------------- */
void SyncData_Init(void);
void SyncData_UpdatePeriod_InIrq(void);
int SYNC_Command_Func(char *CmdStr, char *Cmd);
void AIA_SyncData_Upload(int id);


#endif

























