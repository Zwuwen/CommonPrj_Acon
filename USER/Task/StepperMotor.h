/**
  ******************************************************************************
  * @file    StepperMotor.c 
  * @author  Bowen.He
  * @version V1.0
  * @created    24-April-2019
  * @lastModified 
  * @brief   
  ******************************************************************************
  * @attention
  *
  * 
  * <h2><center>&copy; COPYRIGHT 2015 ACONBIO</center></h2>
  ******************************************************************************/
#ifndef __AIA_STEPPERMOTOR_H
#define	__AIA_STEPPERMOTOR_H

#include "AIA_Utilities.h"
#include "AIA_PID.h"

#define TOTAL_MOTOR_NUMBER 1



typedef enum 
{
	DIR_POSITIVE/*�����˶�*/,
	DIR_NEGATIVE/*�����˶�*/
}_MOTORDIR;


//����������ʱ��������������ϰ���
//�������������ִ�к��ֶ�����һֱ��ͣ�ĳ���ֱ��Ŀ�ĵػ��� ����ֹͣ�ȴ���λ������
typedef enum
{
	RUN_ANYWAY_WHEN_OUTOFSTEP,
	RUN_STOP_WHEN_OUTOFSTEP
	/*,RUN_DEC_WHEN_TROUBLE*/
}_MOTOROUTOFSTEP;


typedef enum 
{
	RUN_EQUALSPD/*�������н׶�*/,
	RUN_ACCSPD/*�������н׶�*/,
	RUN_DECSPD/*�������н׶�*/,
	RUN_STOP_BYDEC/*����ֹͣ���н׶�*/, 
	RUN_STOP_IMMEDIATELY/*����ֹͣ���н׶�*/, 
	RUN_OVER/*���н����׶�*/
}_MOTORRUNSTATE;


typedef enum 
{
	OVER_ORIGIN/*ԭ���Ѿ��ҵ�*/,
	NO_ORIGIN/*ԭ�㻹û����*/
}_MOTORORIGINSTATE;

typedef enum 
{
	MODE_EQUALSPD/*��������ģʽ*/,
	MODE_POSITION/*��λģʽ*/
}_MOTORRUNMODE;


typedef enum 
{
	STOPBYDEC/*����ֹͣģʽ*/, 
	STOPIMMEDIATELY/*����ֹͣģʽ*/
}_MOTORSTOPMODE;


typedef enum 
{
	ABS_COORD/*��������*/, 
	REL_COORD/*�������*/
}_MOTORABSORREL;


typedef enum 
{
	FINDBYSENSOR, 
	FINDBYBLOCK, 
	FINDBYZSIG, 
	FINDBYTTL
}_RSTMODE;


typedef enum 
{
	MOVETYPE_ROTATE	/*ת������*/, 
	MOVETYPE_LINE	/*ֱ���˶�����*/
}_MOTORMOVETYPE;

typedef enum 
{
	T_CURVE	/*T�ͼ���*/, 
	S_CURVE	/*S�ͼ���*/,
	P_CURVE /*�����߼���*/
}_MOTORACCMODE;


typedef enum
{
	MICRO1 = 1, 
	MICRO2 = 2, 
	MICRO4 = 4, 
	MICRO8 = 8, 
	MICRO16 = 16,
	MICRO32 = 32
}_MOTORMICROTYPE;
#define MICROSTEP  	MICRO8




typedef struct _MOTOR
{
	int Acc;
	int Dec;
	int SpdStart;
	int SpdMax;
	int SpdRst;
	int SoftLimitP;
	int SoftLimitN;
	int SpdLevel;
	
	int CurrSpd;
	int CurrAbsPos;
	int PosPurpose;
	int TotalPulse;
	int PulseEscape;
	
	int PulseForAcc;
	int PulseForEqualSpd;
	int PulseForDec;
	int OriginOffset;

/*����������*/	
	int EncodeCount;
	int EncodeCircles;
	int EncodeErrOffset;
	int EncodeCurrAbsPos; /*��ȡ������ʱ�򣬵�ǰ����ֵ*/
	int sMicroStep; /*static*/
	
	int ActionCode;
	
	_MOTORDIR Dir;
	_MOTORRUNSTATE RunState;
	_MOTORORIGINSTATE OriginState;
	_MOTORRUNMODE RunMode;
	_MOTORSTOPMODE StopMode;
	_MOTORMICROTYPE MicroType;
	_MOTORMOVETYPE MoveType;
	_MOTOROUTOFSTEP OutofStepMode;
	_MOTORACCMODE AccMode;
	
	/*Flags during process*/
	union{ 
		struct{
			unsigned enablePwmAdjust	:1; 
			unsigned firstEnablePwm     :1; 
			unsigned stopByCmd			:1;
			unsigned taskCompletion		:1; 
			unsigned HasEncoder			:1;
			unsigned EnableSensorLimitP :1;
			unsigned EnableSensorLimitN :1;	
			unsigned Bit4		:1;
			unsigned Bit5		:1;
			unsigned Bit6		:1; 
			unsigned Bit7		:1;

		}Bit;
		int Allbits;
	}flag;
	
	void (*PWM_Enable)(void);
	void (*PWM_Disable)(void);
	void (*Motor_Enable)(void);
	void (*Motor_Disable)(void);
	BOOL (*GetPositiveSensor)(void);
	BOOL (*GetNegativeSensor)(void);
	void (*SetMotorDir)(_MOTORDIR Dir);
	BOOL (*PWM_Pulse_Change)(int Freqs);
	
}MOTOR;

extern MOTOR StepperMotor[TOTAL_MOTOR_NUMBER];




#endif
  


































