#include "TemperatureTask.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"
#include "AIA_Persistence.h"
#include "AD7708_SPI.h"

int ReadingTemp[TOTAL_PID_NUMBER];
int SetPointTemp[TOTAL_PID_NUMBER];

//TODO :读取温度， 设置温度， 读取偏移量 设置偏移量
//  偏移量 = 设置值- 实测值     比如 设置37度，如果测得反应盘温度 36度 则 偏移量 = 37-36  = 1 度

void TemperatureDataInit(void)
{
	int i;
	
	for(i=0;i<TOTAL_PID_NUMBER;++i)
	{
		SetPointTemp[i] = ((-1000<PersistenceParams.TargetValue[i])&&(6000>PersistenceParams.TargetValue[i])) ? PersistenceParams.TargetValue[i] : 4000;
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
		ReadingTemp[i] = ReadChannleTemperature(g_aAdcChannel[i]);	
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
	
	CHECK_RANGE_PARAM_1(0,1);
	CHECK_RANGE_PARAM_2(0,TOTAL_PID_NUMBER-1);

	val[0] = module->recvParams[0];
	val[1] = module->recvParams[1];
	
	PrepareResponseBuf(module, "%d", ((TEMPERATURE_OFFSET)? ReadingTemp[val[0]] - PersistenceParams.OffSet[val[0]]: ReadingTemp[val[0]]));
	
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
	
	SetPointTemp[val[1]] = ((TEMPERATURE_OFFSET)? val[2] + PersistenceParams.OffSet[val[1]] : val[2]);
		
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
	
	PrepareResponseBuf(module, "%d", PersistenceParams.OffSet[val[0]]);
	
	return PREPARE_IN_PROCESS;	
	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QD_Process(AIAMODULE *module)/*set offset*/
{
	int val[3];
	
	CHECK_RANGE_PARAM_1(0,TOTAL_PID_NUMBER-1);
	CHECK_RANGE_PARAM_2(-50,50);
	
	val[0] = module->recvParams[0];
	val[1] = module->recvParams[1];
	val[2] = module->recvParams[2];
	
	PersistenceParams.OffSet[val[0]] = val[1];

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





































