#include "tim.h"

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

/* TIM3 init function */
void TIM3_Init(void)//100ms
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = TIM3PRE;//预分频
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = TIM3NUM;	//周期
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_NVIC_SetPriority(TIM3_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

//  HAL_TIM_Base_Start_IT(&htim3);
}

void TIM6_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  __HAL_RCC_TIM6_CLK_ENABLE();

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = TIM6PRE;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = TIM6NUM;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
  HAL_NVIC_SetPriority(TIM6_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(TIM6_IRQn);

}

void Buzzer_Start(void)
{
    HAL_TIM_Base_Start_IT(&htim6);   // 使能TIM6及其中断
}

void Buzzer_Stop(void)
{
    HAL_TIM_Base_Stop_IT(&htim6);
    BUZ_POWER_RESET; // 拉低，确保MOS管关断
}



