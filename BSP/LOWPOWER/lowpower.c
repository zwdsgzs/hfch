#include "lowpower.h"

void PowerOn_4G(void)
{
    LTE_POWER_SET;//上电
	LTE_PWRKEY_SET;
	LTE_RST_SET;
	LTE_DTR_SET;
	HAL_Delay(35);
	LTE_PWRKEY_RESET;
	HAL_Delay(720);
	LTE_PWRKEY_SET;
}

void PowerOff_4G(void)
{
    LTE_PWRKEY_RESET;
	HAL_Delay(750);
	LTE_PWRKEY_SET;
	HAL_Delay(10000);
	LTE_POWER_RESET;
	LTE_PWRKEY_RESET;
	LTE_RST_RESET;
	LTE_DTR_RESET;
}

void PowerRST_4G(void)
{
    LTE_RST_RESET;
	HAL_Delay(320);
	LTE_RST_SET;
}

void Sensor_PowerOn(void)
{
	V3_6_POWER_SET;
	BQ35100_Init();//电量库仑计,上电
    PowerOn_4G();
	CH4_POWER_SET;
	H2S_POWER_SET;
	RADAR_POWER_SET;
}

void Sensor_PowerOff(void)
{
	CH4_POWER_RESET;
	H2S_POWER_RESET;
	RADAR_POWER_RESET;
    PowerOff_4G();
	LED_POWER_RESET;

	GE_RESET;
	V3_6_POWER_RESET;
}

void sleep_pin_Power_cfg(void)
{
    /* 关所有传感器电源 */
    Sensor_PowerOff();

	/* 清空缓冲区 */
	ring_buf_clear(&rb_usart1);
	ring_buf_clear(&rb_usart2);
	ring_buf_clear(&rb_usart4);
	ring_buf_clear(&rb_usart5);

	/* 关闭串口中断 */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    HAL_NVIC_DisableIRQ(USART4_5_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);

	/* 关闭定时器中断 */
	HAL_TIM_Base_Stop_IT(&htim3);

    /*  关闭不需要的 GPIO 时钟（省电） */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    //__HAL_RCC_GPIOB_CLK_DISABLE();	// 加速度唤醒引脚,
    // __HAL_RCC_GPIOC_CLK_DISABLE();   // 磁开关唤醒引脚
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();

    /* 关闭不需要的外设时钟 */
    __HAL_RCC_USART1_CLK_DISABLE();
    __HAL_RCC_USART2_CLK_DISABLE();
    __HAL_RCC_USART4_CLK_DISABLE();
    __HAL_RCC_USART5_CLK_DISABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();
    __HAL_RCC_TIM3_CLK_DISABLE(); 
}

void wkp_pin_Power_cfg(void)
{
    /* 重新打开 GPIO 时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    /* 重新打开外设时钟 */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_USART4_CLK_ENABLE();
    __HAL_RCC_USART5_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_TIM3_CLK_ENABLE();

	/* 清空缓冲区 */
	ring_buf_clear(&rb_usart1);
	ring_buf_clear(&rb_usart2);
	ring_buf_clear(&rb_usart4);
	ring_buf_clear(&rb_usart5);

    /* 重新打开串口中断 */
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_EnableIRQ(USART4_5_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

	/*  重新打开定时器中断 */
	HAL_TIM_Base_Start_IT(&htim3);
    /* 重新上电传感器 */
    Sensor_PowerOn();
	HDC2010_Init();//温湿度
	IIS2DLPC_Init();//加速度计
}

void Sleep(void)
{
    /* 清 MQTT 状态 */
    stu_4G.get_frist_Mq_flg = 0;
    stu_4G.opentop = false;

    /* 配置睡眠引脚 */
    sleep_pin_Power_cfg();

    /* 暂停 SysTick（防止它不断唤醒 CPU） */
    HAL_SuspendTick();

    /* 进入 STOP 模式 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

// -------------------------- 被唤醒 ------------------------------- 

    /* 恢复 SysTick */
    HAL_ResumeTick();

    /* 重新配置系统时钟（STOP 唤醒后时钟会切回 MSI/HSI） */
    SystemClock_Config();

	/* 清 GPIO EXTI 标志 */
    HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);

    /* 清 RTC 标志 */
    HAL_NVIC_ClearPendingIRQ(RTC_IRQn);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* 关 RTC 唤醒定时器（如果不需要持续唤醒） */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    /* 恢复引脚和外设 */
    wkp_pin_Power_cfg();

	//计算电量
	BQ35100_ReadAccumulatedCapacity();

	stu_4G.flow_pub_pending = 0;
	stu_flag.bRtcwkp = false;
	Flow_Start();
}


