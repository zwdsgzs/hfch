#include "rtc.h"

RTC_HandleTypeDef hrtc;

void RTC_Init(void)//时钟源，外部慢速晶振
{
    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat     = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv   = 127;
    hrtc.Init.SynchPrediv    = 255;
    hrtc.Init.OutPut         = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }

    /* 使能 RTC 中断 */
    HAL_NVIC_SetPriority(RTC_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}

/**
 * @brief  启动 RTC 唤醒定时器
 * @param  minutes  多少分钟后唤醒
 */
void RTC_StartWakeUp_Minutes(uint32_t minutes)
{
    uint32_t seconds = minutes * 60;

    /* 范围检查：CK_SPRE_16BITS 最大 65535 秒 */
    if (seconds > 65535) seconds = 65535;

    /* 先关掉上次的 */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    /* 启动，计数值 = 秒数 */
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, seconds, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	stu_flag.bRtcwkp = true;
}


