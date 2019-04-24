#include "TemperatureTask.h"
#include "AIA_ModuleCore.h"
#include "AIA_Protocol2.0.h"
#include "AIA_ErrorCode.h"
#include "CAN_Driver.h"
#include "string.h"
#include "AIA_Persistence.h"
#include "AD7708_SPI.h"


int SetTemp[TOTAL_PID_NUMBER];
int TempOffSet[TOTAL_PID_NUMBER];	
int ReadingTemp[TOTAL_PID_NUMBER];



int PerpareToTemperatureArea(char *dest)
{
	int *tmp;
	int i;
	tmp = (int*)dest;
	
	for(i=0; i<TOTAL_PID_NUMBER; i++)
	{
		*tmp++ = SetTemp[i];
		*tmp++ = TempOffSet[i];
	}
	return 32;
}




void TemperatureData_FromPersistence(char *src)
{
	int i;
	int *tmp;
	
	for(i=0; i<TOTAL_PID_NUMBER; ++i)
	{
		SetTemp[i] = *tmp++;
		TempOffSet[i] = *tmp++;
	}
}


/**
  * @brief  GET FROME AD7708 .
  * @param  None
  * @retval None
  */
void GetTemperature(void)
{
	int i;

	for(i=0; i<TOTAL_PID_NUMBER; i++)
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
	CHECK_RANGE_PARAM_1(0, TOTAL_PID_NUMBER -1);
	
	PrepareResponseBuf(module, "%d", ((TEMPERATURE_OFFSET) ? ReadingTemp[PARAM_1] - TempOffSet[PARAM_1] : ReadingTemp[PARAM_1]));
	
	return PREPARE_IN_PROCESS;	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QB_Process(AIAMODULE *module)/*set temperature*/
{
	CHECK_RANGE_PARAM_1(0, TOTAL_PID_NUMBER-1);
	
	CHECK_RANGE_PARAM_2(-1000,6000);

	SetTemp[PARAM_1] = ((TEMPERATURE_OFFSET)? PARAM_2 + TempOffSet[PARAM_1] : PARAM_2);
	
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
	
	PrepareResponseBuf(module, "%d", TempOffSet[val[0]]);
	
	return PREPARE_IN_PROCESS;	
	
}

/**
  * @brief  
  * @param  
  * @retval res
  */
int QD_Process(AIAMODULE *module)/*set offset*/
{
	CHECK_RANGE_PARAM_1(0, TOTAL_PID_NUMBER - 1);
	
	CHECK_RANGE_PARAM_2(-50, 50);
	
	TempOffSet[PARAM_1] = PARAM_2;

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





































