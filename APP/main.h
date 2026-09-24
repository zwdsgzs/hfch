#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

#include "stm32l0xx_it.h"
<<<<<<< HEAD
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
=======
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
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

#define runtime	400	//40秒，4g上电启动要10秒，

// 状态类型定义
typedef enum {
    STATE_RUNNING,
    STATE_STOPPED,
    STATE_CONFIG,
} State_t;

// 状态机结构
typedef struct {
    State_t state;
    int counter;  // 运行时的计数器
}StateMachine;
extern StateMachine sm;

typedef struct{
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

<<<<<<< HEAD
//队列句柄
extern QueueHandle_t xUartEvtQueue;
//信号量句柄
extern SemaphoreHandle_t xTim3Sem;
void appNetTask(void *argument);
void appSlowTask(void *argument);
void appUartTask(void *argument);

=======
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
