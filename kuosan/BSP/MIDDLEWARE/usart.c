#include "usart.h"

stuUartDef stu_uart = {0};

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;

// 为每个串口分配一个独立的单字节接收缓存
unsigned char rx_byte1 = 0;
unsigned char rx_byte2 = 0;
unsigned char rx_byte4 = 0;
unsigned char rx_byte5 = 0;

/* USART1 init function */
void USART1_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**USART1 GPIO Configuration
  PA9     ------> USART1_TX
  PA10     ------> USART1_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1_Init */
  uartHandle->Instance = USART1;
  uartHandle->Init.BaudRate = UART1_BAUD;
  uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandle->Init.StopBits = UART_STOPBITS_1;
  uartHandle->Init.Parity = UART_PARITY_NONE;
  uartHandle->Init.Mode = UART_MODE_TX_RX;
  uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
  uartHandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartHandle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(uartHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

void USART1_DeInit(void)
{
    HAL_UART_DeInit(&huart1);

    // 4. 关键一步：将 TX 和 RX 引脚配置为模拟输入（高阻态）
    // 这样即使 MCU 还在运行，TX 引脚也不会输出 3.3V，彻底断开漏电路径
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG; // 模拟输入模式，最彻底的隔离
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 关闭 USART1 的时钟以省电和彻底复位
    __HAL_RCC_USART1_CLK_DISABLE();
}

void USART1_ReInit(UART_HandleTypeDef *uartHandle)
{
  // 强行复位 USART1 外设硬
  __HAL_RCC_USART1_FORCE_RESET();
  __HAL_RCC_USART1_RELEASE_RESET();

  //重置 HAL 库的状态变量
  huart1.RxState = HAL_UART_STATE_READY;
  huart1.ErrorCode = HAL_UART_ERROR_NONE;

  // 清空 NVIC 挂起标志（防止中断死循环）
  NVIC_ClearPendingIRQ(USART1_IRQn);

  // 重新初始化串口
  USART1_Init(&huart1);

  // 重新开启接收中断
  HAL_UART_Receive_IT(&huart1, &rx_byte1, 1);
}
/* USART2 init function */

void USART2_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**USART2 GPIO Configuration
  PA2     ------> USART2_TX
  PA3     ------> USART2_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART2_Init */
  uartHandle->Instance = USART2;
  uartHandle->Init.BaudRate = UART2_BAUD;
  uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandle->Init.StopBits = UART_STOPBITS_1;
  uartHandle->Init.Parity = UART_PARITY_NONE;
  uartHandle->Init.Mode = UART_MODE_TX_RX;
  uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
  uartHandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartHandle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(uartHandle) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USART4 init function */

void USART4_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_USART4_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /**USART4 GPIO Configuration
  PC10     ------> USART4_TX
  PC11     ------> USART4_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USART4_Init */
  uartHandle->Instance = USART4;
  uartHandle->Init.BaudRate = UART4_BAUD;
  uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandle->Init.StopBits = UART_STOPBITS_1;
  uartHandle->Init.Parity = UART_PARITY_NONE;
  uartHandle->Init.Mode = UART_MODE_TX_RX;
  uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
  uartHandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartHandle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(uartHandle) != HAL_OK)
  {
    Error_Handler();
  }
}
/* USART5 init function */

void USART5_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART5_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /**USART5 GPIO Configuration
  PB3     ------> USART5_TX
  PB4     ------> USART5_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_USART5;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USART5_Init */
  uartHandle->Instance = USART5;
  uartHandle->Init.BaudRate = UART5_BAUD;
  uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandle->Init.StopBits = UART_STOPBITS_1;
  uartHandle->Init.Parity = UART_PARITY_NONE;
  uartHandle->Init.Mode = UART_MODE_TX_RX;
  uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
  uartHandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartHandle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(uartHandle) != HAL_OK)
  {
    Error_Handler();
  }
}

void uartRcvStart(unsigned char uartx)
{
	switch (uartx)
	{
	case 0:
		HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);

		HAL_NVIC_SetPriority(USART4_5_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART4_5_IRQn);

		// 启动单字节接收中断
		HAL_UART_Receive_IT(&huart1, &rx_byte1, 1);
		HAL_UART_Receive_IT(&huart2, &rx_byte2, 1);
		HAL_UART_Receive_IT(&huart4, &rx_byte4, 1);
		HAL_UART_Receive_IT(&huart5, &rx_byte5, 1);
		break;
	case 1:
		HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

		// 启动单字节接收中断
		HAL_UART_Receive_IT(&huart1, &rx_byte1, 1);
		break;
	case 2:
		HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);

		HAL_UART_Receive_IT(&huart2, &rx_byte2, 1);
		break;
	case 4:
		HAL_NVIC_SetPriority(USART4_5_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART4_5_IRQn);

		HAL_UART_Receive_IT(&huart4, &rx_byte4, 1);
		break;
	case 5:
		HAL_NVIC_SetPriority(USART4_5_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(USART4_5_IRQn);

		HAL_UART_Receive_IT(&huart5, &rx_byte5, 1);
		break;
	default:break;
	}
}

/**
 * @brief  按串口编号发送数据（中断方式）
 * @param  uartx  串口编号：1 / 2 / 4 / 5
 * @param  data   数据指针
 * @param  len    数据长度
 * @retval HAL_OK      发送已启动
 *         HAL_BUSY    该串口上一次发送还没完成
 *         HAL_ERROR   串口编号无效或参数错误
 */
HAL_StatusTypeDef uartSendIT(unsigned char uartx, const uint8_t *data, uint16_t len)
{
  UART_HandleTypeDef *huart = NULL;

  if (data == NULL || len == 0) {
      return HAL_ERROR;
  }

  switch (uartx)
  {
  case 1: huart = &huart1; break;
  case 2: huart = &huart2; break;
  case 4: huart = &huart4; break;
  case 5: huart = &huart5; break;
  default: return HAL_ERROR;
  }

  // 检查该串口是否空闲
  if (huart->gState != HAL_UART_STATE_READY) {
      return HAL_BUSY;
  }

  return HAL_UART_Transmit_IT(huart, data, len);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	// 清除所有错误标志
    __HAL_UART_CLEAR_OREFLAG(huart);
    __HAL_UART_CLEAR_NEFLAG(huart);
    __HAL_UART_CLEAR_FEFLAG(huart);
    __HAL_UART_CLEAR_PEFLAG(huart);
    
    // 强行读取 DR 寄存器，物理破除 ORE 锁死状态
    volatile uint32_t tmpreg = huart->Instance->RDR; 
    (void)tmpreg;
    
    huart->ErrorCode = HAL_UART_ERROR_NONE;

	if (huart->Instance == USART1)
	{
        HAL_UART_Receive_IT(huart, &rx_byte1, 1);
	}
	else if (huart->Instance == USART2)
	{
		HAL_UART_Receive_IT(huart, &rx_byte2, 1);
	}
    else if (huart->Instance == USART4)
    {
		HAL_UART_Receive_IT(huart, &rx_byte4, 1);
    }
	else if (huart->Instance == USART5)
	{
		HAL_UART_Receive_IT(huart, &rx_byte5, 1);
	}
}

// 接收完成回调（每收到 1 个字节触发一次）
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t woken = pdFALSE;

    if (huart->Instance == USART1)
    {
        // 将数据压入环形缓冲区
        ring_buf_write(&rb_usart1, &rx_byte1, 1);

        //重新开启接收中断，准备收下一个字节
        HAL_UART_Receive_IT(huart, &rx_byte1, 1);
    }
	else if (huart->Instance == USART2)
	{
	  ring_buf_write(&rb_usart2, &rx_byte2, 1);
	  stu_uart.uchStatus = 2;
	  HAL_UART_Receive_IT(huart, &rx_byte2, 1);
	  xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
	}
	else if (huart->Instance == USART4)
    {
        ring_buf_write(&rb_usart4, &rx_byte4, 1);
        stu_uart.uchStatus = 4;
        HAL_UART_Receive_IT(huart, &rx_byte4, 1);
        xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
    }
    else if (huart->Instance == USART5)
    {
        ring_buf_write(&rb_usart5, &rx_byte5, 1);
        stu_uart.uchStatus = 5;
        HAL_UART_Receive_IT(huart, &rx_byte5, 1);
        xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
    }
    
    portYIELD_FROM_ISR(woken);
}


#if (DEBUG_UART5 == 1)
/* ---------- 调试模式：printf → USART5 ---------- */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, 100);
    return ch;
}
#endif

