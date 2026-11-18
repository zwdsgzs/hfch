/**
 *******************************************************************************
 * @file  main.c
 * @brief Main program.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2024-07-19       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "main.h"
#include "ADC.h"
//#include "hc32_ll_rmu.h"

//int timecnt = 0;
unsigned char connectfail = 0;//连接异常
unsigned char connectfailnext = 0;//连接异常
bool start = true;//设备开关机
bool key_locked = false;//开关锁，防止干簧管抖动造成关不上。
float batv = 0.0;

StateMachine sm = {
	.state = STATE_RUNNING,
	.counter = T_COUNT,
};

/**
 * @brief  Main function of the project
 * @param  None
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    /* Register write unprotected for some required peripherals. */
   LL_PERIPH_WE(LL_PERIPH_GPIO | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_FCG | LL_PERIPH_EFM);

    //Clock Config
    App_ClkCfg();
     //Port Config
    App_PortCfg();
	//唤醒中断
	App_Port_WKP();
	//ADC Config
	App_ADCCfg();
	//USARTx Config
    App_USARTxCfg();
	//RTC Config
    App_RTCCfg_LowPower();
	//STOP Config
	STOP_Config();
    /* Register write protected for some required peripherals. */
    LL_PERIPH_WP(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_EFM);
    printf("software_version=[%d]  hardware_version=[%s]\n", software_version,hardware_version);
	/*--降低功耗，开机时传感器部分都不上电,开机泵响2s提醒---*/
	Flash_Init();//初始化FLASH，有默认的设备三元组
	flash_copy();
	
	V5_SET;
	V3_6_SET;
	RADA_POWER_SET;

	NbIot_PowerOn();
	DDL_DelayMS(4000);
#if LADAR_HLK
	SetSendTime();//设置雷达发送间隔
#endif
	DDL_DelayMS(4000);
	
	connectfail = Init_4G();
	if (connectfail == 0)
	{
		sm.counter = 0;
		sm.state = STATE_STOPPED;
	}
	else{
		connectfailnext = 1;
		get_frist_Mq_flg = 1;
	}
//	start = false;

	for(;;)
	{
		switch (sm.state)        
		{
		case STATE_RUNNING:
			if (start)
			{
				Workjob();
				sm.counter--;
				if (sm.counter == 0)
				{
					sm.state = STATE_STOPPED;
				}
			}
			else{
				sm.counter = 0;
				sm.state = STATE_STOPPED;
			}
			break;
		case STATE_STOPPED:
			key_locked = false;
			if (stu_sensor.uchLDCnt == 0)
			{
				printf("sleep:%d min\n", ParaTemp.updateIntervalmins);
				App_RTC_SetAlarm(ParaTemp.updateIntervalmins);
			}else{
				printf("sleep:1 min\n");
				App_RTC_SetAlarm(1);
			}
			Sleep();
			Wake();
			sm.state = STATE_RUNNING;
			sm.counter = T_COUNT;
//			start = true;
			key_locked = false;
			break;
		case STATE_CONFIG:
			if (stu_Uart.uchReply != 0)
			{
				if (stu_Uart.uchReply == 2)
				{
					NVIC_SystemReset();
				}
				replyData(stu_Uart.uchReply);
				stu_Uart.uchReply = 0;
			}
			DDL_DelayMS(100);
			if ((stu_Uart.prodid) && (stu_Uart.dn) && (stu_Uart.devname) && (stu_Uart.devSecret))
			{
				ParaWrite();
				stu_Uart.prodid = false;
				stu_Uart.dn = false;
				stu_Uart.devname = false;
				stu_Uart.devSecret = false;
			}
			break;
		default:break;
		}
		LED_Toggle;
		DDL_DelayMS(1000);
	}
}



void Workjob(void)
{
	switch (sm.counter)
	{
	case T_COUNT:
		if ((connectfail) && (connectfailnext))
		{
			V5_SET;
			V3_6_SET;
			MOTOR_POWER_SET;
		}
		break;
	case T_CONNECT:
		if (get_frist_Mq_flg == 0)
		{
			if (connectfail == 0)
			{
				connectfail = Init_4G();
				if (connectfail == 0)
				{
					sm.counter = 1;
//					sm.state = STATE_STOPPED;
				}
				else{
					get_frist_Mq_flg = 1;
					sm.counter = T_COUNT +1;
					connectfailnext = 1;
				}
			}
			else{
				connectfailnext = reConnect_4G();
				if (connectfailnext == 0)
				{
					sm.counter = 1;
//					sm.state = STATE_STOPPED;
				}
				else{
					get_frist_Mq_flg = 1;
					sm.counter = T_COUNT+1;
				}
			}
		}
		break;
	case T_SENSOR_ON:
		if (stu_sensor.open)
		{
			Mq_Pub_open();//只要触发就上报，不清除，等待人员处理后在平台手动清
			stu_sensor.open = false;
		}
		GPS_POWER_SET;
		RADA_POWER_SET;
		CH4_POWER_SET;
		H2S_POWER_SET;
		PA3_REF_RESET;
		break;
	case T_MOTOR_OFF:
		MOTOR_POWER_RESET;
		break;
	case T_H2S_0:
		stu_sensor.uchH2SCnt = 0;
		H2S_SendRead();
		break;
	case T_RADAR_0:
		#if !LADAR_HLK
			LD8001_Read();
		#endif
		break;
	case T_H2S_1:
		stu_sensor.uchH2SCnt = 1;
		H2S_SendRead();
		break;
	case T_RADAR_1:
		#if !LADAR_HLK
			LD8001_Read();
		#endif
		break;
	case 11:
		batv = Get_Adc(get_battery); 
		if(batv > 3.05)
		{
			stu_sensor.batter = (unsigned short)(((batv-3.05) / (3.65-3.05)) * 100);
		}
		else{
			stu_sensor.batter = 0;
		}
		PA3_REF_SET;
		break;
	case T_H2S_2:
		stu_sensor.uchH2SCnt = 2;
		H2S_SendRead();
		break;
	case T_RADAR_2:
		#if !LADAR_HLK
			LD8001_Read();
		#endif
		break;
	case T_H2S_3:
		stu_sensor.uchH2SCnt = 3;
		H2S_SendRead();
		break;
	case T_RADAR_3:
		#if !LADAR_HLK
			LD8001_Read();
		#endif
		break;
	case T_H2S_4:
		stu_sensor.uchH2SCnt = 4;
		H2S_SendRead();
		break;
	case T_RADAR_4:
		#if !LADAR_HLK
			LD8001_Read();
		#endif
		break;
	case T_PUB:
		Mq_Pub_P();//发送JSON
		break;
	case T_SUB:
		Mq_Sub_shadow();//设置订阅期望回复，以及发送期望指令
		break;
	}
}



