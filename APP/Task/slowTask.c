#include "main.h"

/**
* @brief Function implementing the slowTask thread.
* @param argument: Not used
* @retval None
*/

void appSlowTask(void *argument)
{
	unsigned char uchCount = 0;

////-------------设备初始化----------------------
//	HDC2010_Init();//温湿度
//	IIS2DLPC_Init();//加速度计
////-------------初始逻辑----------------------
	Sensor_PowerOn();
//	stu_4G.InitFlag = 0;
//	Flow_Start();
	
	for(;;)
	{
		if (xSemaphoreTake(xTim3Sem, portMAX_DELAY) == pdTRUE)
		{
			uchCount ++;
			if (uchCount % 5 == 0)
			{
				LED_POWER_TOGGLE;
			}
			
			if(uchCount >= 200)
			{
				uchCount = 0;
			}
		}
	}
//  osDelay(1);
}


