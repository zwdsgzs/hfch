/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
//------------------传感器-----------------
#define CH4_PORT			GPIOA
#define CH4_PIN				GPIO_PIN_15
#define CH4_POWER_SET   	HAL_GPIO_WritePin(CH4_PORT, CH4_PIN, GPIO_PIN_SET)
#define CH4_POWER_RESET 	HAL_GPIO_WritePin(CH4_PORT, CH4_PIN, GPIO_PIN_RESET)

#define H2S_PORT			GPIOB
#define H2S_PIN				GPIO_PIN_2
#define H2S_POWER_SET   	HAL_GPIO_WritePin(H2S_PORT, H2S_PIN, GPIO_PIN_SET)
#define H2S_POWER_RESET   	HAL_GPIO_WritePin(H2S_PORT, H2S_PIN, GPIO_PIN_RESET)

#define RADAR_PORT			GPIOB
#define RADAR_PIN			GPIO_PIN_9
#define RADAR_POWER_SET     HAL_GPIO_WritePin(RADAR_PORT, RADAR_PIN, GPIO_PIN_SET)
#define RADAR_POWER_RESET   HAL_GPIO_WritePin(RADAR_PORT, RADAR_PIN, GPIO_PIN_RESET)

//------------------LED-----------------
#define LED_PORT			GPIOA
#define LED_PIN				GPIO_PIN_4
#define LED_POWER_SET   	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET)
#define LED_POWER_RESET 	HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET)
#define LED_POWER_TOGGLE 	HAL_GPIO_TogglePin(LED_PORT, LED_PIN)

//------------------4G-----------------
#define LTE_EN_PORT			GPIOB
#define LTE_EN_PIN			GPIO_PIN_1
#define LTE_POWER_SET   	HAL_GPIO_WritePin(LTE_EN_PORT, LTE_EN_PIN, GPIO_PIN_SET)
#define LTE_POWER_RESET   	HAL_GPIO_WritePin(LTE_EN_PORT, LTE_EN_PIN, GPIO_PIN_RESET)

#define LTE_PWRKEY_PORT		GPIOC
#define LTE_PWRKEY_PIN		GPIO_PIN_9
#define LTE_PWRKEY_SET   	HAL_GPIO_WritePin(LTE_PWRKEY_PORT, LTE_PWRKEY_PIN, GPIO_PIN_SET)
#define LTE_PWRKEY_RESET   	HAL_GPIO_WritePin(LTE_PWRKEY_PORT, LTE_PWRKEY_PIN, GPIO_PIN_RESET)

#define LTE_RST_PORT		GPIOC
#define LTE_RST_PIN			GPIO_PIN_8
#define LTE_RST_SET   		HAL_GPIO_WritePin(LTE_RST_PORT, LTE_RST_PIN, GPIO_PIN_SET)
#define LTE_RST_RESET   	HAL_GPIO_WritePin(LTE_RST_PORT, LTE_RST_PIN, GPIO_PIN_RESET)

#define LTE_DTR_PORT		GPIOA
#define LTE_DTR_PIN			GPIO_PIN_8
#define LTE_DTR_SET   		HAL_GPIO_WritePin(LTE_DTR_PORT, LTE_DTR_PIN, GPIO_PIN_SET)
#define LTE_DTR_RESET   	HAL_GPIO_WritePin(LTE_DTR_PORT, LTE_DTR_PIN, GPIO_PIN_RESET)

//------------------电源-----------------
#define V3_6_EN_PORT		GPIOA
#define V3_6_EN_PIN			GPIO_PIN_1
#define V3_6_POWER_SET   	HAL_GPIO_WritePin(V3_6_EN_PORT, V3_6_EN_PIN, GPIO_PIN_SET)
#define V3_6_POWER_RESET   	HAL_GPIO_WritePin(V3_6_EN_PORT, V3_6_EN_PIN, GPIO_PIN_RESET)

#define GE_PORT				GPIOC
#define GE_PIN				GPIO_PIN_0
#define GE_SET   			HAL_GPIO_WritePin(GE_PORT, GE_PIN, GPIO_PIN_SET)
#define GE_RESET   			HAL_GPIO_WritePin(GE_PORT, GE_PIN, GPIO_PIN_RESET)

#define BAT_INT_PORT		GPIOB
#define BAT_INT_PIN			GPIO_PIN_5

//------------------加速度计-----------------
#define ACC_INT_PORT		GPIOB
#define ACC_INT_PIN			GPIO_PIN_8

//------------------温湿度计-----------------
#define TH_INT_PORT			GPIOC
#define TH_INT_PIN			GPIO_PIN_13

//------------------磁开关-----------------
#define MAGN_INT_PORT		GPIOC
#define MAGN_INT_PIN		GPIO_PIN_12

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void FREEGPIO_Init(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

