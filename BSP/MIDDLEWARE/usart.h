/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define RX_BUFFER_SIZE RING_BUF_SIZE


#define UART1_BAUD	115200
#define UART2_BAUD	9600
#define UART4_BAUD	115200
#define UART5_BAUD	115200


typedef struct{
	unsigned char uchStatus;
}stuUartDef;
extern stuUartDef stu_uart;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void USART1_Init(UART_HandleTypeDef *uartHandle);
void USART2_Init(UART_HandleTypeDef *uartHandle);
void USART4_Init(UART_HandleTypeDef *uartHandle);
void USART5_Init(UART_HandleTypeDef *uartHandle);

/* USER CODE BEGIN Prototypes */

void uartRcvStart(unsigned char uartx);
HAL_StatusTypeDef uartSendIT(unsigned char uartx, const uint8_t *data, uint16_t len);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

