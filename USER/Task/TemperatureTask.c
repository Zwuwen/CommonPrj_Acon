#include "TemperatureTask.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"
#include "AIA_Persistence.h"
#include "AD7708_SPI.h"

//_TEMPERATURE_PARAM TemperParams;
int SetTemp[TOTAL_PID_NUMBER];
int OffSet[TOTAL_PID_NUMBER];	

int ReadingTemp[TOTAL_PID_NUMBER];

//TODO :读取温度， 设置温度， 读取偏移量 设置偏移量
//  偏移量 = 设置值- 实测值     比如 设置37度，如果测得反应盘温度 36度 则 偏移量 = 37-36  = 1 度

#define PSIS_LENGTH  20
int PerpareToPLL(char *src)
{
	int *dest
		
	 dest = (int*)src;
	*dest++ = SetTemp[0];
	*dest++ = SetTemp[1];
	*dest++ = SetTemp[2];
	*dest++ = SetTemp[3];
	
	*dest++ = OffSet[0];
	*dest++ = OffSet[1];
	*dest++ = OffSet[2];
	*dest++ = OffSet[3];
}

int PerpareToTemper(int IsPersisValid)
{
	if(IsPersisValid)
	{
		memcpy(&TemperParams.SetPointTemp, &PersistenceParams.temper,sizeof(PersistenceParams.temper));
	}
}

void TemperatureDataInit(void)
{
	int i;
	
	for(i=0; i<TOTAL_PID_NUMBER; ++i)
	{
		TemperParams.SetPointTemp[i] = PersistenceParams.TEMPERATURE_PARAM.TargetValue[i];
		TemperParams.OffSet[i] = PersistenceParams.OffSet[i];
	}
}
/**
  * @brief  GET FROME AD7708 .
  * @param  None
  * @retval None
  */
void GetTemperature(void)
{
	u8 i;

	for(i=0;i<TOTAL_PID_NUMBER;i++)
	{
		TemperParams.ReadingTemp[i] = ReadChannleTemperature(g_aAdcChannel[i]);	
	}
}
/**
  * @brief  
  * @param  
  * @retval res
  */
int QA_Process(AIAMODULE *module)/*read temperature*/
{
	int val[2];
	
	CHECK_RANGE_PARAM_1(0, 1);
	CHECK_RANGE_PARAM_2(0, TOTAL_PID_NUMBER-1);

	val[0] = module->recvParams[0];
	val[1] = module->recvParams[1];
	
	PrepareResponseBuf(module, "%d", ((TEMPERATURE_OFFSET)? TemperParams.ReadingTemp[val[0]] - TemperParams.OffSet[val[0]]: TemperParams.ReadingTemp[val[0]]));
	
	return PREPARE_IN_PROCESS;	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QB_Process(AIAMODULE *module)/*set temperature*/
{
	int val[3];
	
	CHECK_RANGE_PARAM_1(0,1);
	CHECK_RANGE_PARAM_2(0,TOTAL_PID_NUMBER-1);
	CHECK_RANGE_PARAM_3(-1000,6000);
	
	val[0] = module->recvParams[0];
	val[1] = module->recvParams[1];
	val[2] = module->recvParams[2];
	
	TemperParams.SetPointTemp[val[1]] = ((TEMPERATURE_OFFSET)? val[2] + TemperParams.OffSet[val[1]] : val[2]);
		
	return PASS;	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QC_Process(AIAMODULE *module)/*Read offset*/
{
	int val[1];
	
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	
	val[0] = module->recvParams[0];
	
	PrepareResponseBuf(module, "%d", TemperParams.PersisParam.OffSet[val[0]]);
	
	return PREPARE_IN_PROCESS;	
	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QD_Process(AIAMODULE *module)/*set offset*/
{
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	CHECK_RANGE_PARAM_2(-50,50);
	
	TemperParams.OffSet[PARAM_1] = PARAM_2;

	return PASS;	
}




int TemperatureTask_CmdProcess(AIAMODULE *module, int cmdword)
{
	int ret;
	
	switch(cmdword)
	{
		CASE_REGISTER_CMD_PROCESS(QA);	/*read temperature*/
		CASE_REGISTER_CMD_PROCESS(QB);	/*set temperature*/
		CASE_REGISTER_CMD_PROCESS(QC);	/*Read offset*/
		CASE_REGISTER_CMD_PROCESS(QD);	/*Set offset*/
		
		default:
			ret = ERR_CMDNOTIMPLEMENT;
		break;		
	}
	
	return ret;
}





































