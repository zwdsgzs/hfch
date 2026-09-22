#include "main.h"

osThreadId_t netTaskHandle;
const osThreadAttr_t netTask_attributes = {
  .name = "netTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for slowTask */
osThreadId_t slowTaskHandle;
const osThreadAttr_t slowTask_attributes = {
  .name = "slowTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

SemaphoreHandle_t xTim3Sem = NULL;
QueueHandle_t xUartEvtQueue;

stuFlagDef  stu_flag = {0};
StateMachine sm = {
	.state = STATE_RUNNING,
	.counter = 0,
};

int main(void)
{
//-------------外设初始化--------------------
	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
//	USART1_Init(&huart1);//4G
//	USART2_Init(&huart2);//硫化氢 ASCII，长度不固定，发送读取
	USART4_Init(&huart4);//激光甲烷	ASCII,长度固定,自动上传
//	USART5_Init(&huart5);//雷达	modbus rtu 长度固定，发送读取
	I2C1_Init();
	TIM3_Init();
	RTC_Init();
//-------------flash数据初始化----------------------
	flashReadFromEEPROM();
	ringBufInit(0);//初始化环形缓冲区

	/* Init scheduler */
	osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
	
	xTim3Sem = xSemaphoreCreateBinary();
	// 创建队列：16 条消息，每条 1 字节
    xUartEvtQueue = xQueueCreate(16, sizeof(stu_uart));

	netTaskHandle = osThreadNew(appNetTask, NULL, &netTask_attributes);
	slowTaskHandle = osThreadNew(appSlowTask, NULL, &slowTask_attributes);
	uartTaskHandle = osThreadNew(appUartTask, NULL, &uartTask_attributes);

	HAL_TIM_Base_Start_IT(&htim3);
	osKernelStart();

	while (1)
	{
	}
}

void runMode(void)
{
//	switch (sm.counter)
//	{
//	case 50://激光甲烷4.5秒自动上报一次
//		stu_sensor.collectCnt = 0;
//		H2S_Read();
//		LD8001_Read();
//		HDC2010TriggerMeasurement();
//		IIS2DLPC_GetTiltAngle();
//		break;
//	case 100:
//		stu_sensor.collectCnt = 1;
//		H2S_Read();
//		LD8001_Read();
//		HDC2010TriggerMeasurement();
//		IIS2DLPC_GetTiltAngle();
//		break;
//	case 150:
//		stu_sensor.collectCnt = 2;
//		H2S_Read();
//		LD8001_Read();
//		HDC2010TriggerMeasurement();
//		IIS2DLPC_GetTiltAngle();
//		break;
//	case 200:
//		stu_sensor.collectCnt = 3;
//		H2S_Read();
//		LD8001_Read();
//		HDC2010TriggerMeasurement();
//		IIS2DLPC_GetTiltAngle();
//		break;
//	case 250:
//		stu_sensor.collectCnt = 4;
//		H2S_Read();
//		LD8001_Read();
//		HDC2010TriggerMeasurement();
//		IIS2DLPC_GetTiltAngle();
//		break;
//	case 300:
//		Mqtt_RequestPublish();
//		break;
//	default:break;
//	}

//    AT_Task();
//    Flow_Task();

//	if ((sm.counter >= runtime) || (stu_4G.flow_pub_pending == 3))//模式切换
//	{
//		sm.state = STATE_STOPPED;
//		sm.counter = 0;
//	}

//	if (stu_flag.uchBatterFlag != 0)//电池引脚中断，低电量
//	{
//	}
//	if (stu_flag.uchMagnetFlag != 0)//磁开关引脚中断，关机
//	{
//		sm.state = STATE_STOPPED;
//		sm.counter = 0;
//		stu_flag.uchMagnetFlag = 0;
//	}
//	if (stu_flag.uchTempHumFlag != 0)//温湿度引脚中断,转换完成
//	{
//		HDC2010ReadTemp();
//		HDC2010ReadHumidity();
//	}

	if (sm.counter%10 == 0)
	{
		LED_POWER_TOGGLE;
	}
}

void stopMode(void)
{
	Sleep();
	sm.state = STATE_RUNNING;
	sm.counter = 0;
}

void cfgMode(void)
{
	
}

