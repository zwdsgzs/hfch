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
	unsigned char uchBuzCnt = 0;
	unsigned char uchBuzTarget = 0; //变量记录当前的蜂鸣器目标时长
	bool bsleep = false;
	
	HAL_TIM_Base_Start_IT(&htim3);

//-------------设备初始化----------------------
	HDC2010_Init();//温湿度
	IIS2DLPC_Init();//加速度计
////-------------初始逻辑----------------------
	Sensor_PowerOn();
	stu_flag.bAccFlag = false;
	BQ35100_ReadAccumulatedCapacity();//计算电量
	IIS2DLPC_GetTiltAngle();
	
	for(;;)
	{
		if (xSemaphoreTake(xTim3Sem, portMAX_DELAY) == pdTRUE)
		{
			usCount ++;

			if (usCount == 2)//蜂鸣器
			{
				uchBuzCnt = 1;
				uchBuzTarget = 12;//开机长响：12 * 100ms = 1200ms
			}
			if (uchBuzCnt != 0)
			{
				if (uchBuzCnt == 1)
				{
					Buzzer_Start();
				}

				uchBuzCnt++;

				if (uchBuzCnt >= uchBuzTarget)
				{
					Buzzer_Stop();
					uchBuzCnt = 0;
					if (bsleep)
					{
						bsleep = false;
						usCount = 0;
						stu_4G.process = 0;
						if (stu_sensor.uchLDCnt == 0)
						{
							Sleep(stu_flash.updateIntervalmins);
						}
						else{
							Sleep(5);
						}
					}
				}
			}

			if (stu_flag.uchTempHumFlag != 0)//温湿度引脚中断,转换完成
			{
				HDC2010ReadTemp();
				HDC2010ReadHumidity();
			}

			if ((usCount >= 50) && (usCount < 300))
			{
				if (usCount % 10 == 0)
				{
					uchStep = (usCount / 10) % 5;
					switch (uchStep)
					{
					case 0:
						if (usCount == 50)
						{
							ClearSensorData(&stu_sensor);
						}
						stu_sensor.collectCnt = (usCount / 50) - 1;
						break;
					case 1:
						H2S_Read();
						break;
					case 2:
#if (DEBUG_UART5 == 0)
						if (!stu_Uart.state)
						{
							LD8001_Read();
						}
#endif
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

			if (usCount == 200)
			{
				stu_4G.process = 1;
			}
			else if (usCount == 300)
			{
				stu_4G.process = 2;
			}

			if (usCount % 5 == 0)
			{
				LED_POWER_TOGGLE;
			}
			if (!stu_Uart.state)
			{
				//	运行超时				磁开关关机						4g上报完成
				if ((usCount >= 600) || (stu_flag.uchMagnetFlag != 0) || (stu_4G.process == 4))
				{
					if (!bsleep) // 防止重复进入分支
					{
						bsleep = true;        // 标记进入关机流程
						uchBuzCnt = 1;        // 如果此时蜂鸣器没响，强制触发
						uchBuzTarget = 4;     // 关机短响：4 * 100ms = 400ms
						if (stu_flag.uchMagnetFlag != 0)
						{
							uchBuzCnt = uchBuzTarget;
						}
					}
				}
			}
			else{
				if ((stu_Uart.prodid) && (stu_Uart.dn) && (stu_Uart.devname) && (stu_Uart.devSecret))
				{
					flashWriteToEEPROM();
					stu_Uart.prodid = false;
					stu_Uart.dn = false;
					stu_Uart.devname = false;
					stu_Uart.devSecret = false;
					osDelay(50);
					__disable_irq();
					HAL_NVIC_SystemReset();
				}

				if (usCount >= 600)
				{
					usCount = 0;
				}
			}
		}
	}
//  osDelay(1);
}


