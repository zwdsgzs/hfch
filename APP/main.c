#include "main.h"

<<<<<<< HEAD
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
=======
stuFlagDef  stu_flag = {0}; 
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
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
<<<<<<< HEAD
//	USART1_Init(&huart1);//4G
//	USART2_Init(&huart2);//硫化氢 ASCII，长度不固定，发送读取
	USART4_Init(&huart4);//激光甲烷	ASCII,长度固定,自动上传
//	USART5_Init(&huart5);//雷达	modbus rtu 长度固定，发送读取
=======
	USART1_Init(&huart1);//4G
	USART2_Init(&huart2);//硫化氢 ASCII，长度不固定，发送读取
	USART4_Init(&huart4);//激光甲烷	ASCII,长度固定,自动上传
	USART5_Init(&huart5);//雷达	modbus rtu 长度固定，发送读取
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
	I2C1_Init();
	TIM3_Init();
	RTC_Init();
//-------------flash数据初始化----------------------
	flashReadFromEEPROM();
<<<<<<< HEAD
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
=======
//-------------设备初始化----------------------
	Sensor_PowerOn();
	HDC2010_Init();//温湿度
	IIS2DLPC_Init();//加速度计
//-------------初始逻辑----------------------
	ringBufInit(0);//初始化环形缓冲区
	uartRcvStart(0);//使能串口接收
	stu_4G.InitFlag = 0;
	Flow_Start();
	
	while (1)
	{
		switch (sm.state)        
		{
		case STATE_RUNNING:
			runMode();
			break;
		case STATE_STOPPED:
			stopMode();
			break;
		case STATE_CONFIG:
			cfgMode();
			break;
		}
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
	}
}

void runMode(void)
{
<<<<<<< HEAD
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
=======
	switch (sm.counter)
	{
	case 50://激光甲烷4.5秒自动上报一次
		stu_sensor.collectCnt = 0;
		H2S_Read();
		LD8001_Read();
		HDC2010TriggerMeasurement();
		IIS2DLPC_GetTiltAngle();
		break;
	case 100:
		stu_sensor.collectCnt = 1;
		H2S_Read();
		LD8001_Read();
		HDC2010TriggerMeasurement();
		IIS2DLPC_GetTiltAngle();
		break;
	case 150:
		stu_sensor.collectCnt = 2;
		H2S_Read();
		LD8001_Read();
		HDC2010TriggerMeasurement();
		IIS2DLPC_GetTiltAngle();
		break;
	case 200:
		stu_sensor.collectCnt = 3;
		H2S_Read();
		LD8001_Read();
		HDC2010TriggerMeasurement();
		IIS2DLPC_GetTiltAngle();
		break;
	case 250:
		stu_sensor.collectCnt = 4;
		H2S_Read();
		LD8001_Read();
		HDC2010TriggerMeasurement();
		IIS2DLPC_GetTiltAngle();
		break;
	case 300:
		Mqtt_RequestPublish();
		break;
	default:break;
	}

    AT_Task();
    Flow_Task();

	if ((sm.counter >= runtime) || (stu_4G.flow_pub_pending == 3))//模式切换
	{
		sm.state = STATE_STOPPED;
		sm.counter = 0;
	}

	if (stu_uart.uchStatus != 0)//串口有数据
	{
		switch (stu_uart.uchStatus)
		{
//		case 1:
//			break;
		case 2:
			H2Srcv();
			break;
		case 4:
			laserCH4rcv();
			break;
		case 5:
			Radarrcv();
			break;
		}
		stu_uart.uchStatus = 0;
	}

	if (stu_flag.uchBatterFlag != 0)//电池引脚中断，低电量
	{
	}
	if (stu_flag.uchMagnetFlag != 0)//磁开关引脚中断，关机
	{
		sm.state = STATE_STOPPED;
		sm.counter = 0;
		stu_flag.uchMagnetFlag = 0;
	}
	if (stu_flag.uchTempHumFlag != 0)//温湿度引脚中断,转换完成
	{
		HDC2010ReadTemp();
		HDC2010ReadHumidity();
	}

	if (sm.counter%5 == 0)
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
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

