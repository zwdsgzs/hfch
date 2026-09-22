#include "main.h"

/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_appUartTask */
void appUartTask(void *argument)
{
	uartRcvStart(0);//使能串口接收

	for(;;)
	{
		if (xQueueReceive(xUartEvtQueue, &stu_uart, portMAX_DELAY) == pdTRUE)
        {
            switch (stu_uart.uchStatus)
            {
            case 2:
                if (ring_buf_available(&rb_usart2) > 0) 
				{
                    H2Srcv();
                }
                break;
            case 4:
                if (ring_buf_available(&rb_usart4) > 0) 
				{
                    laserCH4rcv();
                }
                break;
            case 5:
                if (ring_buf_available(&rb_usart5) > 0) 
				{
                    Radarrcv();
                }
                break;
            default:
                break;
            }
        }
	}
}


