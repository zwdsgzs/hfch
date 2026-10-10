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

int main(void)
{
//-------------外设初始化--------------------
	HAL_Init();
	SystemClock_Config();
	GPIO_Init();
	USART1_Init(&huart1);//4G
	USART2_Init(&huart2);//硫化氢 ASCII，长度不固定，发送读取
	USART4_Init(&huart4);//激光甲烷	ASCII,长度固定,自动上传
	USART5_Init(&huart5);//雷达	modbus rtu 长度固定，发送读取
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

	osKernelStart();

	while (1)
	{
	}
}


