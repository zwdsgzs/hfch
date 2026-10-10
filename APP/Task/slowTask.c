#include "main.h"

/**
* @brief Function implementing the slowTask thread.
* @param argument: Not used
* @retval None
*/

void appSlowTask(void *argument)
{
	unsigned short usCount = 0;
	unsigned char uchStep = 0; 
	
	HAL_TIM_Base_Start_IT(&htim3);

//-------------设备初始化----------------------
	HDC2010_Init();//温湿度
	IIS2DLPC_Init();//加速度计
////-------------初始逻辑----------------------
	Sensor_PowerOn();
	stu_flag.bAccFlag = false;
	IIS2DLPC_GetTiltAngle();
	
	for(;;)
	{
		if (xSemaphoreTake(xTim3Sem, portMAX_DELAY) == pdTRUE)
		{
			usCount ++;

			if (stu_flag.uchTempHumFlag != 0)//温湿度引脚中断,转换完成
			{
				HDC2010ReadTemp();
				HDC2010ReadHumidity();
			}
			if (stu_flag.bAccFlag)
			{
				stu_flag.bAccFlag = false;
				stu_flag.bUpport = true;
				IIS2DLPC_GetTiltAngle();
			}

			if ((usCount >= 50) && (usCount < 300))
			{
				if (usCount % 10 == 0)
				{
					uchStep = (usCount / 10) % 5;
					switch (uchStep)
					{
					case 0:
						stu_sensor.collectCnt = (usCount / 50) - 1;
						BQ35100_ReadAccumulatedCapacity();
						break;
					case 1:
						H2S_Read();
						break;
					case 2:
						LD8001_Read();
						break;
					case 3:
						HDC2010TriggerMeasurement();
						break;
					case 4:
						IIS2DLPC_GetTiltAngle();
						break;
					default:break;
					}
				}
			}

			if (usCount % 5 == 0)
			{
				LED_POWER_TOGGLE;
			}
			//	运行超时				磁开关关机						4g上报完成
			if ((usCount >= 500) || (stu_flag.uchMagnetFlag != 0) || (stu_4G.reported == 2))
			{
				usCount = 400;
				stu_4G.reported = 0;
				if (stu_sensor.uchLDCnt == 0)
				{
					Sleep(5);
				}
				else{
					Sleep(stu_flash.updateIntervalmins);
				}
			}
		}
	}
//  osDelay(1);
}


