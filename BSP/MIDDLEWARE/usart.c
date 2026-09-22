#include "usart.h"

stuUartDef stu_uart = {0};

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart4_rx;
DMA_HandleTypeDef hdma_usart5_rx;

unsigned char uart1_rxbuffer[RX_BUFFER_SIZE] = {0};
unsigned char uart2_rxbuffer[RX_BUFFER_SIZE] = {0};
unsigned char uart4_rxbuffer[RX_BUFFER_SIZE] = {0};
unsigned char uart5_rxbuffer[RX_BUFFER_SIZE] = {0};

/* USART1 init function */
void USART1_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
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

  /* USART1 DMA Init */
  /* USART1_RX Init */
  hdma_usart1_rx.Instance = DMA1_Channel3;
  hdma_usart1_rx.Init.Request = DMA_REQUEST_3;
  hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
  hdma_usart1_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

  // 使能 USART 空闲中断
  __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
}
/* USART2 init function */

void USART2_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
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
   
  /* USART2 DMA Init */
  /* USART2_RX Init */
  hdma_usart2_rx.Instance = DMA1_Channel5;
  hdma_usart2_rx.Init.Request = DMA_REQUEST_4;
  hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
  hdma_usart2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
  if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

  // 使能 USART 空闲中断
  __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
}
/* USART4 init function */

void USART4_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_USART4_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
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
  
  /* USART4 DMA Init */
  /* USART4_RX Init */
  hdma_usart4_rx.Instance = DMA1_Channel6;
  hdma_usart4_rx.Init.Request = DMA_REQUEST_12;
  hdma_usart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_usart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart4_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart4_rx.Init.Mode = DMA_CIRCULAR;
  hdma_usart4_rx.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_usart4_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart4_rx);

  // 使能 USART 空闲中断
//  __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
}
/* USART5 init function */

void USART5_Init(UART_HandleTypeDef *uartHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_USART5_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
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

  /* USART5 DMA Init */
  /* USART5_RX Init */
  hdma_usart5_rx.Instance = DMA1_Channel2;
  hdma_usart5_rx.Init.Request = DMA_REQUEST_13;
  hdma_usart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_usart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_usart5_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_usart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_usart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_usart5_rx.Init.Mode = DMA_CIRCULAR;
  hdma_usart5_rx.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_usart5_rx) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart5_rx);

  // 使能 USART 空闲中断
  __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
}

void uartRcvStart(unsigned char uartx)
{
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  HAL_NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

  HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);

  HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);

  HAL_NVIC_SetPriority(USART4_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART4_5_IRQn);

	switch (uartx)
	{
	case 0:
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rxbuffer, RX_BUFFER_SIZE);
//		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_rxbuffer, RX_BUFFER_SIZE);
//		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_rxbuffer, RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart4_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
//		HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_rxbuffer, RX_BUFFER_SIZE);
//		__HAL_DMA_DISABLE_IT(&hdma_usart5_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		break;
	case 1:
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rxbuffer, RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		break;
	case 2:
		HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_rxbuffer, RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		break;
	case 4:
		HAL_UARTEx_ReceiveToIdle_DMA(&huart4, uart4_rxbuffer, RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart4_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		break;
	case 5:
		HAL_UARTEx_ReceiveToIdle_DMA(&huart5, uart5_rxbuffer, RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(&hdma_usart5_rx, DMA_IT_HT);  // 关闭半传输中断，避免频繁触发
		break;
	default:break;
	}

	// DMA1_Channel6 的 CCR 寄存器 EN 位
	if ((DMA1_Channel6->CCR & DMA_CCR_EN) == 0)
	{
		return;
		// DMA 通道没使能！
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
	if (huart->Instance == USART1)
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
	}
	else if (huart->Instance == USART2)
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
	}
    else if (huart->Instance == USART4)
    {
//		uint32_t err = huart->ErrorCode;
        // HAL_UART_ERROR_PE   = 0x01  校验错误
        // HAL_UART_ERROR_NE   = 0x02  噪声错误
        // HAL_UART_ERROR_FE   = 0x04  帧错误
        // HAL_UART_ERROR_ORE  = 0x08  溢出错误
        // HAL_UART_ERROR_DMA  = 0x10  DMA传输错误
        // HAL_UART_ERROR_RTO  = 0x20  接收超时

        // 清除所有错误标志
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
    }
	else if (huart->Instance == USART5)
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        __HAL_UART_CLEAR_PEFLAG(huart);
        huart->ErrorCode = HAL_UART_ERROR_NONE;
	}
}
int leng = 0;
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	static uint16_t uart1size = 0;
	static uint16_t uart2size = 0;
	static uint16_t uart4size = 0;
	static uint16_t uart5size = 0;
  BaseType_t woken = pdFALSE;

  if (huart->Instance == USART1)
  {
	  if (Size > uart1size)
	  {
		  /* 没绕圈 */
		  ring_buf_write(&rb_usart1, &uart1_rxbuffer[uart1size], Size - uart1size);
	  }
	  else if (Size < uart1size)
	  {
		  /* 绕圈了，分两段写 */
		  ring_buf_write(&rb_usart1, &uart1_rxbuffer[uart1size], RX_BUFFER_SIZE - uart1size);
		  ring_buf_write(&rb_usart1, uart1_rxbuffer, Size);
	  }
	  uart1size = Size;
	  if (uart1size >= RX_BUFFER_SIZE) 
	  {
		  uart1size = 0;
	  }
//	  stu_uart.uchStatus = 1;
  }
  else if (huart->Instance == USART2)
  {
	  if (Size > uart2size)
	  {
		  /* 没绕圈 */
		  ring_buf_write(&rb_usart2, &uart2_rxbuffer[uart2size], Size - uart2size);
	  }
	  else if (Size < uart2size)
	  {
		  /* 绕圈了，分两段写 */
		  ring_buf_write(&rb_usart2, &uart2_rxbuffer[uart2size], RX_BUFFER_SIZE - uart2size);
		  ring_buf_write(&rb_usart2, uart2_rxbuffer, Size);
	  }
	  uart2size = Size;
	  if (uart2size >= RX_BUFFER_SIZE)
	  {
		  uart2size = 0;
	  }
	  stu_uart.uchStatus = 2;
    xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
  }
  else if (huart->Instance == USART4)
  {
	  if (Size > uart4size)
	  {
		  /* 没绕圈 */
		  ring_buf_write(&rb_usart4, &uart4_rxbuffer[uart4size], Size - uart4size);
	  }
	  else if (Size < uart4size)
	  {
		  /* 绕圈了，分两段写 */
		  ring_buf_write(&rb_usart4, &uart4_rxbuffer[uart4size], RX_BUFFER_SIZE - uart4size);
		  ring_buf_write(&rb_usart4, uart4_rxbuffer, Size);
	  }
	  leng = Size - uart4size;
	  uart4size = Size;
	  if (uart4size >= RX_BUFFER_SIZE) 
	  {
		  uart4size = 0;
	  }
	  stu_uart.uchStatus = 4;
    xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
  }
  else if (huart->Instance == USART5)
  {
	  if (Size > uart5size)
	  {
		  /* 没绕圈 */
		  ring_buf_write(&rb_usart5, &uart5_rxbuffer[uart5size], Size - uart5size);
	  }
	  else if (Size < uart5size)
	  {
		  /* 绕圈了，分两段写 */
		  ring_buf_write(&rb_usart5, &uart5_rxbuffer[uart5size], RX_BUFFER_SIZE - uart5size);
		  ring_buf_write(&rb_usart5, uart5_rxbuffer, Size);
	  }
	  uart5size = Size;
	  if (uart5size >= RX_BUFFER_SIZE)
	  {
		  uart5size = 0;
	  }
	  stu_uart.uchStatus = 5;
    xQueueSendFromISR(xUartEvtQueue, &stu_uart, &woken);
  }
}
