#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

#include "stm32l0xx_it.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "rtc.h"
#include "sysclk.h"
#include <cJSON.h>

#include "dataProcess.h"

#include "ch4.h"
#include "h2s.h"
#include "radar.h"
#include "EC800M.h"

#include "HDC2010.h"
#include "bq35100.h"
#include "IIS2DLP.h"

#include "eeprom.h"

#include "lowpower.h"

#include <stdbool.h>

/* =========================================================
 *  USART5 用途选择
 *      1 = 调试打印（PC 串口助手）
 *      0 = 传感器通信
 * ========================================================= */
#define DEBUG_UART5     1

/* 调试宏：只有当 DEBUG_UART5 == 1 时才有效 */
#if (DEBUG_UART5 == 1)
    #define DBG_LOG(fmt, ...)    printf(fmt, ##__VA_ARGS__)
#else
    #define DBG_LOG(fmt, ...)    ((void)0)
#endif

typedef struct{
	bool bUpport;//上报标志，接收到加速度后，等待上报时上报

	bool bAccFlag;//加速计中断标志
	bool bRtcwkp;//RTC中断标志

    unsigned char uchTempHumFlag;//温湿度转换完成标志
    unsigned char uchBatterFlag;//电池没有低电量标志
	unsigned char uchMagnetFlag;//磁开关标志
}stuFlagDef;
extern stuFlagDef  stu_flag;

void runMode(void);
void stopMode(void);
void cfgMode(void);

//队列句柄
extern QueueHandle_t xUartEvtQueue;
//信号量句柄
extern SemaphoreHandle_t xTim3Sem;
void appNetTask(void *argument);
void appSlowTask(void *argument);
void appUartTask(void *argument);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
