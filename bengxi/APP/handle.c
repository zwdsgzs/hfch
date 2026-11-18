#include "main.h"
#include <stdio.h>
#include "config.h"

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  KEY5 External interrupt callback function
 * @param  None
 * @retval None
 */
void KEY_IrqHandle(void)
{
    /* Clear flag */
    EXTINT_ClearExtIntStatus(KEY_EXTINT_CH);
    /* Clear pending */
    NVIC_ClearPendingIRQ(KEY_INT_IRQn);
    __DSB();
//	NVIC_SystemReset();

	/* 抖动锁定期内直接返回 */
    if (key_locked) 
	{
        return;
    }
	
	//锁死，开始防抖窗口
	key_locked = true;

	// 翻转开关状态
    start = !start;
}

/**
 * @brief  KEY5 External interrupt callback function
 * @param  None
 * @retval None
 */
void Q_CHECK_IrqHandle(void)
{
//    printf("KEY5 handle occurs!\r\n");
//	DDL_DelayMS(1500);
    /* Clear flag */
    EXTINT_ClearExtIntStatus(Q_CHECK_EXTINT_CH);
    /* Clear pending */
    NVIC_ClearPendingIRQ(Q_CHECK_INT_IRQn);
    __DSB();
	
	stu_sensor.open = true;
//	if(GET_Q_CHECK==0&&Factory_mode==1)
//	{
//		Stop_work=1;
//	}
}


/**
 * @brief  TMR0 compare interrupt callback function.
 * @param  None
 * @retval None
 */
//void TMR0_CompareIrqCallback(void)
//{
////    timecnt ++;

//    TMR0_ClearStatus(TMR0_UNIT, TMR0_CH_FLAG);
//}

/* INT_SRC_USART6_RI Callback. */
void USART6_Handler(void)
{
	if(USART_GetStatus(CM_USART6,USART_FLAG_RX_FULL))
	{
		USART_ClearStatus(CM_USART6,USART_FLAG_RX_FULL);
		//add your codes here
		unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART6);	
		LADAR_Rcv_One_Char(u16Data);
	}
}

/* INT_SRC_USART5_RI Callback. */
void USART5_Handler(void)
{
	if(USART_GetStatus(CM_USART5,USART_FLAG_RX_FULL))
	{
		USART_ClearStatus(CM_USART5,USART_FLAG_RX_FULL);
    //add your codes here
		unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART5);
		H2S_Rcv_One_Char(u16Data);
	}
}
//static unsigned char  Data[2];
/* INT_SRC_USART4_RI Callback. */
void USART4_Handler(void)
{
	if(USART_GetStatus(CM_USART4,USART_FLAG_RX_FULL))
	{
		USART_ClearStatus(CM_USART4,USART_FLAG_RX_FULL);
		unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART4);
		USART_WriteData(CM_USART1,u16Data);
		EC_01_Rcv_One_Char(u16Data);
	}
}

/* INT_SRC_USART3_RI Callback. */
void USART3_Handler(void)
{
	if(USART_GetStatus(CM_USART3,USART_FLAG_RX_FULL))
	{
		//add your codes here
		USART_ClearStatus(CM_USART3,USART_FLAG_RX_FULL);
//		while (USART_GetStatus(CM_USART3, USART_FLAG_RX_FULL))
		{
			unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART3);
			GP_02_Rcv_One_Char(u16Data);
		}
	}
}

/* INT_SRC_USART2_RI Callback. */
void USART2_Handler(void)
{
	if(USART_GetStatus(CM_USART2,USART_FLAG_RX_FULL))
	{
		USART_ClearStatus(CM_USART2,USART_FLAG_RX_FULL);
		//add your codes here
		unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART2);
		//USART_WriteData(CM_USART1,u16Data);
		YL_WI_YY_6001_Rcv_One_Char(u16Data);
	}
}

/* INT_SRC_USART1_RI Callback. */
void USART1_Handler(void)
{
	if(USART_GetStatus(CM_USART1,USART_FLAG_RX_FULL))
	{
		USART_ClearStatus(CM_USART1,USART_FLAG_RX_FULL);
		//add your codes here
		unsigned char u16Data=(uint8_t)USART_ReadData(CM_USART1);
		TTL_Rcv_One_Char(u16Data);
	}
}

/* INT_SRC_USART5_EI Callback. */
void USART5_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART5);

    USART_ClearStatus(CM_USART5, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/* INT_SRC_USART2_EI Callback. */
void USART2_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART2);

    USART_ClearStatus(CM_USART2, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));	//
}

/* INT_SRC_USART6_EI Callback. */
void USART6_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART6);

    USART_ClearStatus(CM_USART6, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));	
}

/* INT_SRC_USART4_EI Callback. */
void USART4_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART4);

    USART_ClearStatus(CM_USART4, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));	
}

/* INT_SRC_USART1_EI Callback. */
void USART1_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART1);

    USART_ClearStatus(CM_USART1, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

/* INT_SRC_USART3_EI Callback. */
void USART3_Err_Handler(void)
{
    //add your codes here
	(void)USART_ReadData(CM_USART3);

	// 读取错误状态
    int err_status = USART_GetStatus(CM_USART3, (USART_FLAG_PARITY_ERR | 
                                                          USART_FLAG_FRAME_ERR | 
                                                          USART_FLAG_OVERRUN));

    USART_ClearStatus(CM_USART3, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));	
}

/* INT_SRC_RTC_PRD Callback. */
void RTC_Handler(void)
{
    //add your codes here
//   RTCN++;
//	printf("-----rtc-------\n");
}

/* 闹钟中断处理 - 低功耗版本 */
void RTC_AlarmHandler(void)
{
//    stc_rtc_alarm_t stcRtcAlarm;
    
    /* 清除闹钟标志 */
    RTC_ClearStatus(RTC_FLAG_ALARM);
	 NVIC_ClearPendingIRQ(INT000_IRQn);
}

void SysTick_Handler(void)//SysTick是一个24位的递减计数器,当 SysTick计数到0时,会产生一个中断，此时会调用 SysTick_Handler 函数
{
    SysTick_IncTick();

    __DSB();  /* Arm Errata 838869 */
}


uint32_t User_Time_Read(uint32_t time)
{
    if(SysTick_GetTick()>=time)
    {
       return SysTick_GetTick()-time;
    }
    return(0xFFFFFFFF-time+SysTick_GetTick());
}
void User_Time_Set(uint32_t* time)
{
   *time=SysTick_GetTick();
}


void HardFault_Handler(void)
{
	while(1)
	{
//		printf("HardFault！！！");
		SysTick_Delay(1000U);
		__disable_irq();
		NVIC_SystemReset();
	}
}


/**
 * @}
 */
#pragma import(__use_no_semihosting) 
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	USART_WriteData(CM_USART1, (uint8_t)ch);
    while(RESET == USART_GetStatus(CM_USART1, USART_FLAG_TX_EMPTY));//传输数据寄存器空标志
	return ch;
}


/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
