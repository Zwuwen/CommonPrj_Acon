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
	DIR_POSITIVE/*正向运动*/,
	DIR_NEGATIVE/*反向运动*/
}_MOTORDIR;


//带编码器的时候如果遇到运行障碍，
//比如带不动，将执行何种动作，一直不停的尝试直到目的地还是 立即停止等待上位机处理
typedef enum
{
	RUN_ANYWAY_WHEN_OUTOFSTEP,
	RUN_STOP_WHEN_OUTOFSTEP
	/*,RUN_DEC_WHEN_TROUBLE*/
}_MOTOROUTOFSTEP;


typedef enum 
{
	RUN_EQUALSPD,
	RUN_ACCSPD,
	RUN_DECSPD,
	RUN_STOP_BYDEC, 
	RUN_STOP_IMMEDIATELY, 
	RUN_OVER,
}_MOTORRUNSTATE;


typedef enum 
{
	OVER_ORIGIN/*原点已经找到*/,
	NO_ORIGIN/*原点还没有找*/
}_MOTORORIGINSTATE;

typedef enum 
{
	MODE_EQUALSPD/*匀速运行模式*/,
	MODE_POSITION/*定位模式*/
}_MOTORRUNMODE;

typedef enum 
{
	STEP_CALCULATE,
	STEP_MOTORRUN,
	STEP_ADJUSTPOSITION,
	STEP_IDLE,
}_MOTORRUNSTEP;


typedef enum 
{
	STOPBYDEC/*减速停止模式*/, 
	STOPIMMEDIATELY/*立即停止模式*/
}_MOTORSTOPMODE;


typedef enum 
{
	ABS_COORD/*绝对坐标*/, 
	REL_COORD/*相对坐标*/
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
	MOVETYPE_ROTATE	/*转盘类型*/, 
	MOVETYPE_LINE	/*直线运动类型*/
}_MOTORMOVETYPE;

typedef enum 
{
	T_CURVE	/*T型加速*/, 
	S_CURVE	/*S型加速*/,
	P_CURVE /*抛物线加速*/
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

/*编码器参数*/	
	int EncodeCount;
	int EncodeCircles;
	int EncodeErrOffset;
	int EncodeCurrAbsPos; /*读取编码器时候，当前脉冲值*/
	int sMicroStep; /*static*/
	int sEncodePPR; /*static*/
	
	int ActionCode;
	
	float PulseEncodeRatio; /*for calcuate only*/
	
	TIM_TypeDef* EncoderTIM;
	TIM_TypeDef* PwmTIM;
	
	_MOTORDIR Dir;
	_MOTORRUNSTATE RunState;
	_MOTORORIGINSTATE OriginState;
	_MOTORRUNMODE RunMode;
	_MOTORSTOPMODE StopMode;
	_MOTORMICROTYPE MicroType;
	_MOTORMOVETYPE MoveType;
	_MOTOROUTOFSTEP OutofStepMode;
	_MOTORACCMODE AccMode;
	_MOTORRUNSTEP Step;
	
	/*Flags during process*/
	union{ 
		struct{
			unsigned enablePwmAdjust	:1; 
			unsigned firstEnablePwm     :1; 
			unsigned time1ms		    :1; 
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



/* Exported functions ------------------------------------------------------- */
void StepperMotor_Init(void);
void StepperMotor_InMainLoop(void);

#define MotorFindOriginBySensor(pMOTOR) MotorFindOrigin(pMOTOR, FINDBYSENSOR)
#define MotorFindOriginByBlock(pMOTOR) MotorFindOrigin(pMOTOR, FINDBYBLOCK)
#define MotorFindOriginByZsig(pMOTOR) MotorFindOrigin(pMOTOR, FINDBYZSIG)

int MotorGotoA(MOTOR *m, int totalPulse, _MOTORABSORREL absOrRel);
int MotorFindOrigin(MOTOR *m, _RSTMODE rstmode);
void MotorAdjustRunningParamPerMS(MOTOR *m);
int MotorRun(MOTOR *m, _MOTORDIR Dir);
#endif
  


































