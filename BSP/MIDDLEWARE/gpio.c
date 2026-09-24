#include "gpio.h"

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/

void GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*CH4_EN GPIO pins */
  GPIO_InitStruct.Pin = CH4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(CH4_PORT, &GPIO_InitStruct);

  /*H2S GPIO pins */
  GPIO_InitStruct.Pin = H2S_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(H2S_PORT, &GPIO_InitStruct);

  /*RADAR_EN GPIO pins */
  GPIO_InitStruct.Pin = RADAR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(RADAR_PORT, &GPIO_InitStruct);

  /*LED GPIO pins */
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  /*4G_EN GPIO pins  */
  GPIO_InitStruct.Pin = LTE_EN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LTE_EN_PORT, &GPIO_InitStruct);

  /*4G_PWRKEY GPIO pins */
  GPIO_InitStruct.Pin = LTE_PWRKEY_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LTE_PWRKEY_PORT, &GPIO_InitStruct);

  /*4G_RESET GPIO pins  */
  GPIO_InitStruct.Pin = LTE_RST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LTE_RST_PORT, &GPIO_InitStruct);

  /*4G_DTR GPIO pins  */
  GPIO_InitStruct.Pin = LTE_DTR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(LTE_DTR_PORT, &GPIO_InitStruct);

  /*3.6V_EN GPIO pins : PB13 */
  GPIO_InitStruct.Pin = V3_6_EN_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(V3_6_EN_PORT, &GPIO_InitStruct);

  /*BQ35100_EN GPIO pins */
  GPIO_InitStruct.Pin = GE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GE_PORT, &GPIO_InitStruct);

  /*BATTER_INT GPIO pins */
  GPIO_InitStruct.Pin = BAT_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BAT_INT_PORT, &GPIO_InitStruct);

  /*加速度计INT GPIO pins */
  GPIO_InitStruct.Pin = ACC_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ACC_INT_PORT, &GPIO_InitStruct);

  /*温湿度计INT GPIO pins */
  GPIO_InitStruct.Pin = TH_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TH_INT_PORT, &GPIO_InitStruct);

  /*磁开关INT GPIO pins */
  GPIO_InitStruct.Pin = MAGN_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MAGN_INT_PORT, &GPIO_InitStruct);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  FREEGPIO_Init();
}

/* USER CODE BEGIN 2 */
void FREEGPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /*Free GPIO pin :
	PA0
	PA5
	PA6
	PA7
	PA11
	PA12
	*/
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Free GPIO pin :
	PB0
	PB10
	PB11
	PB12
	PB13
	PB14
	PB15
	*/
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Free GPIO pin :
	PC1
	PC2
	PC3
	PC4
	PC5
	PC6
	PC7
	*/
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}
/* USER CODE END 2 */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case BAT_INT_PIN:   // PB5  - BQ35100 ALERT
			stu_flag.uchBatterFlag = 1;
            break;
        case ACC_INT_PIN:   // PB8  - 加速度计
			stu_flag.bAccFlag = true;
            break;
        case TH_INT_PIN:    // PB13 - 温湿度计
			stu_flag.uchTempHumFlag = 1;
            break;
        case MAGN_INT_PIN:  // PB12 - 磁开关
			stu_flag.uchMagnetFlag = 1;
            break;
        default:
            break;
    }
}


