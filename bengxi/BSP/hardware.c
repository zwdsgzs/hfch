#include "hardware.h"

uint8_t Factory_mode=0;
uint8_t MOTOR_START=1;
//uint8_t MOTOR_START=1,CH4_START=0,sleep_time=0,Factory_mode=0;
uint8_t CH4_timeout=0,H2S_timeout=0,RADAR_timeout=0,o2_co_timeout=0,RS485_timeout=0;
uint8_t MOTOR_flag=0; //判断气泵堵塞  0--OK；1--err
//uint16_t RTC_Time_MOTOR_Stamp =0;


//Clock Config	系统主频率8M，定时器按照备注应是500ms
void App_ClkCfg(void)
{
    /* Set bus clock div. */
    CLK_SetClockDiv(CLK_BUS_CLK_ALL, (CLK_HCLK_DIV1 | CLK_EXCLK_DIV2 | CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | \
                                   CLK_PCLK2_DIV4 | CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2));
    /* flash read wait cycle setting */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE0);
    /* XTAL config */
    stc_clock_xtal_init_t stcXtalInit;
    (void)CLK_XtalStructInit(&stcXtalInit);
    stcXtalInit.u8State = CLK_XTAL_ON;
    stcXtalInit.u8Drv = CLK_XTAL_DRV_HIGH;
    stcXtalInit.u8Mode = CLK_XTAL_MD_OSC;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);
    /* 0 cycles for 0MHz ~ 50MHz */
    GPIO_SetReadWaitCycle(GPIO_RD_WAIT0);
    /* Switch driver ability */
    PWC_LowSpeedToHighSpeed();
    /* Set the system clock source */
    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_XTAL);
}



//Port Config此部分包含3段引脚定义，1.串口，2.开关使能，3.其他：ADC，晶振等
void App_PortCfg(void)
{
    /* GPIO initialize */
    stc_gpio_init_t stcGpioInit;
	/* PC14 set to XTAL32-IN */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_14, &stcGpioInit);//晶振

    /* PC15 set to XTAL32-OUT */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_15, &stcGpioInit);

    /* PH0 set to XTAL-IN */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_H, GPIO_PIN_00, &stcGpioInit);

    /* PH1 set to XTAL-EXT/XTAL-OUT */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_H, GPIO_PIN_01, &stcGpioInit);


	/* PH2 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_H, GPIO_PIN_02, &stcGpioInit);//5V_EN

    /* PA0 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
//    stcGpioInit.u16PinState = PIN_STAT_RST;
	stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_00, &stcGpioInit);//CH4 Sensor_EN

	/* PA3 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_03, &stcGpioInit);//分压

	/* PA4 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_04, &stcGpioInit);//RADA_EN

	/* PA6 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_06, &stcGpioInit);//3V6_EN

    /* PA7 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_07, &stcGpioInit);//电机 MOTOR_EN

    /* PC4 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_04, &stcGpioInit);//LED

    /* PB1 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_01, &stcGpioInit);//EEROM_EN

    /* PB2 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_02, &stcGpioInit);//Sensor_H2S_EN

    /* PD2 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_SET;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_D, GPIO_PIN_02, &stcGpioInit);//NB_RST

    /* PB5 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_05, &stcGpioInit);//NB_WAKEU

    /* PB6 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;//设置引脚驱动强度为中等。驱动强度影响引脚输出电流的能力，进而影响外接负载的驱动能力。
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_06, &stcGpioInit);//NB_EN

    /* PB7 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDrv = PIN_MID_DRV;
    stcGpioInit.u16PinAttr = PIN_ATTR_DIGITAL;
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_07, &stcGpioInit);//GPS_EN


	App_FreePortCfg();
}


//Port Config空闲引脚
void App_FreePortCfg(void)
{
	/* GPIO initialize */
	stc_gpio_init_t stcGpioInit;

	/* PC13 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_13, &stcGpioInit);

	/* PC2 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_02, &stcGpioInit);

	/* PC3 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_03, &stcGpioInit);

	/* PA5 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_05, &stcGpioInit);

	/* PB10 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_10, &stcGpioInit);

	/* PC6 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_06, &stcGpioInit);

	/* PC7 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_07, &stcGpioInit);

	/* PC8 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_08, &stcGpioInit);

	/* PC9 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_09, &stcGpioInit);

	/* PA8 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_08, &stcGpioInit);

	/* PA9 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_09, &stcGpioInit);

	/* PA10 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_10, &stcGpioInit);

	/* PC11 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_11, &stcGpioInit);

	/* PC12 set to GPIO-In tput */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
	(void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_12, &stcGpioInit);
}

void App_Port_WKP(void)
{
	/* GPIO initialize */
	stc_gpio_init_t stcGpioInit;
	stc_extint_init_t stcExtIntInit;
	stc_irq_signin_config_t stcIrqSignConfig;

	/* GPIO config */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
	stcGpioInit.u16PullUp = PIN_PU_ON;
	(void)GPIO_Init(KEY_PORT, KEY_PIN, &stcGpioInit);

	/* ExtInt config */
	(void)EXTINT_StructInit(&stcExtIntInit);
	stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;//下降沿，高-低触发
	(void)EXTINT_Init(KEY_EXTINT_CH, &stcExtIntInit);

	/* IRQ sign-in */
	stcIrqSignConfig.enIntSrc    = KEY_INT_SRC;
	stcIrqSignConfig.enIRQn      = KEY_INT_IRQn;
	stcIrqSignConfig.pfnCallback = &KEY_IrqHandle;
	(void)INTC_IrqSignIn(&stcIrqSignConfig);

	/* Enable interrupt function */
	INTC_IntCmd(KEY_INT, ENABLE);
	/* Disable event function */
	INTC_EventCmd(KEY_EVT, DISABLE);
	/* NVIC config */
	NVIC_ClearPendingIRQ(KEY_INT_IRQn);
	NVIC_SetPriority(KEY_INT_IRQn, DDL_IRQ_PRIO_02);
	NVIC_EnableIRQ(KEY_INT_IRQn);

	/* Q_CHECKGPIO config */
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
	stcGpioInit.u16PullUp = PIN_PU_ON;
	(void)GPIO_Init(GPIO_PORT_A, GPIO_PIN_11, &stcGpioInit);//Q_CHECK

	/* ExtInt config */
	(void)EXTINT_StructInit(&stcExtIntInit);
	stcExtIntInit.u32Edge = EXTINT_TRIG_FALLING;//下降沿
	(void)EXTINT_Init(Q_CHECK_EXTINT_CH, &stcExtIntInit);

	/* IRQ sign-in */
	stcIrqSignConfig.enIntSrc    = Q_CHECK_INT_SRC;
	stcIrqSignConfig.enIRQn      = Q_CHECK_INT_IRQn;
	stcIrqSignConfig.pfnCallback = &Q_CHECK_IrqHandle;
	(void)INTC_IrqSignIn(&stcIrqSignConfig);

	/* Enable interrupt function */
	INTC_IntCmd(Q_CHECK_INT, ENABLE);
	/* Disable event function */
	INTC_EventCmd(Q_CHECK_EVT, DISABLE);
	/* NVIC config */
	NVIC_ClearPendingIRQ(Q_CHECK_INT_IRQn);
	NVIC_SetPriority(Q_CHECK_INT_IRQn, DDL_IRQ_PRIO_02);
	NVIC_EnableIRQ(Q_CHECK_INT_IRQn);
}

//ADC Config
void App_ADCCfg(void)
{
	stc_gpio_init_t stcGpioInit;
	stc_adc_init_t stcAdcInit;

	/* PC0 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_00, &stcGpioInit);//BAT_TEST

	/* PC1 set to GPIO-Output */
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(GPIO_PORT_C, GPIO_PIN_01, &stcGpioInit);//外接电源检测
	
    /* 1. Enable ADC1 peripheral clock. */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_ADC1, ENABLE);

    /* 2. Modify the default value depends on the application. */
    (void)ADC_StructInit(&stcAdcInit);
    stcAdcInit.u16ScanMode = ADC_MD_SEQA_SINGLESHOT;
    stcAdcInit.u16Resolution = ADC_RESOLUTION_12BIT;
    stcAdcInit.u16DataAlign = ADC_DATAALIGN_RIGHT;

    /* 3. Initializes ADC. */
    (void)ADC_Init(CM_ADC1, &stcAdcInit);

    /* 4. ADC sequence configuration. */
	/* 4.1 Set the ADC pin to analog input mode. */
	
    /* ADC sequence A configuration. */
	ADC_ChCmd(CM_ADC1, ADC_SEQ_A, ADC_CH10, ENABLE);
	ADC_ChCmd(CM_ADC1, ADC_SEQ_A, ADC_CH11, ENABLE);
    /* ADC Int configuration */
    ADC_IntCmd(CM_ADC1, ADC_INT_EOCA | ADC_INT_EOCB, DISABLE);
}


/**
 * @brief  Configure TMR0.
 * @note   In asynchronous clock, If you want to write a TMR0 register, you need to wait for
 *         at least 3 asynchronous clock cycles after the last write operation!
 * @param  None
 * @retval None
 */
//void TMR0_Config(void)//8000000/256=31.25khz	1/31250=32us	32us*15563=498ms
//{
//    stc_tmr0_init_t stcTmr0Init;
//    stc_irq_signin_config_t stcIrqSignConfig;

//    /* Enable timer0 and AOS clock */
//    FCG_Fcg2PeriphClockCmd(TMR0_CLK, ENABLE);

//    /* TIMER0 configuration */
//    (void)TMR0_StructInit(&stcTmr0Init);
//    stcTmr0Init.u32ClockSrc     = TMR0_CLK_SRC_INTERN_CLK;//实测挂载在pclk1上速度为4m
//    stcTmr0Init.u32ClockDiv     = TMR0_CLK_DIV256;
//    stcTmr0Init.u32Func         = TMR0_FUNC_CMP;
//    stcTmr0Init.u16CompareValue = 15563;
//    (void)TMR0_Init(TMR0_UNIT, TMR0_CH, &stcTmr0Init);
//    /* Asynchronous clock source, writing to TMR0 register requires waiting for three asynchronous clocks. */
//    DDL_DelayMS(1U);
//    TMR0_IntCmd(TMR0_UNIT, TMR0_CH_INT, ENABLE);

//    /* Interrupt configuration */
//    stcIrqSignConfig.enIntSrc    = TMR0_INT_SRC;
//    stcIrqSignConfig.enIRQn      = TMR0_IRQn;
//    stcIrqSignConfig.pfnCallback = &TMR0_CompareIrqCallback;
//    (void)INTC_IrqSignIn(&stcIrqSignConfig);
//    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
//    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
//    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
//}

/**
 * @brief  I2C_Init1 配置i2c的波特率，边沿时长，分频系数，总线命令
 * @param  CM_I2C_TypeDef *I2Cx：I2C1 or I2C2 ,you only need to write a i2c base dar directly
 * @retval LL_OK
 */
//Port Config
void IIC_PortCfg(void)
{
    GPIO_SetDebugPort(GPIO_PIN_SWO, DISABLE);
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_03,GPIO_FUNC_49);//I2C1-SCL
    
    GPIO_SetDebugPort(GPIO_PIN_TRST, DISABLE);
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_04,GPIO_FUNC_48);//I2C1-SDA
    
}
 
 
//I2Cx Config
//void App_I2CxCfg(void)
//{
//    int32_t i32Ret;
//    stc_i2c_init_t stcI2cInit;
//    float32_t fErr;

//	IIC_PortCfg();

//    /* Enable I2C1 clock */
//    FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_I2C1, ENABLE);
//    /************************* Configure I2C1***************************/
//    (void)I2C_DeInit(CM_I2C1);

//    (void)I2C_StructInit(&stcI2cInit);
//    stcI2cInit.u32ClockDiv = I2C_CLK_DIV8;
//    stcI2cInit.u32Baudrate = 50000UL;
//    stcI2cInit.u32SclTime = 0UL;
//    i32Ret = I2C_Init(CM_I2C1, &stcI2cInit, &fErr);
//    if (LL_OK != i32Ret) {
//        //Initialized failed, add your code here.
//    }
//    I2C_BusWaitCmd(CM_I2C1, ENABLE);


//}

/**
 * @brief  Master transmit data
 *
 * @param  [in] u16DevAddr          The slave address
 * @param  [in] au8Data             The data array
 * @param  [in] u32Size             Data size
 * @param  [in] u32Timeout          Time out count
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_TIMEOUT:     Time out
 */
//int32_t I2C_Master_Transmit(uint16_t u16DevAddr, const uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout)
//{
//    int32_t i32Ret;
//    I2C_Cmd(I2C_UNIT, ENABLE);

//    I2C_SWResetCmd(I2C_UNIT, ENABLE);
//    I2C_SWResetCmd(I2C_UNIT, DISABLE);
//    i32Ret = I2C_Start(I2C_UNIT, u32Timeout);
//    if (LL_OK == i32Ret) {

//        i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_TX, u32Timeout);
//        if (LL_OK == i32Ret) {
//            i32Ret = I2C_TransData(I2C_UNIT, au8Data, u32Size, u32Timeout);
//        }
//    }

//    (void)I2C_Stop(I2C_UNIT, u32Timeout);
//    I2C_Cmd(I2C_UNIT, DISABLE);

//    return i32Ret;
//}



/**
 * @brief  Master receive data
 *
 * @param  [in] u16DevAddr          The slave address
 * @param  [in] au8Data             The data array
 * @param  [in] u32Size             Data size
 * @param  [in] u32Timeout          Time out count
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_TIMEOUT:     Time out
 */
//int32_t I2C_Master_Receive(uint16_t u16DevAddr, uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout)
//{
//    int32_t i32Ret;

//    I2C_Cmd(I2C_UNIT, ENABLE);
//    I2C_SWResetCmd(I2C_UNIT, ENABLE);
//    I2C_SWResetCmd(I2C_UNIT, DISABLE);
//    i32Ret = I2C_Start(I2C_UNIT, u32Timeout);
//    if (LL_OK == i32Ret) {
//        if (1UL == u32Size) {
//            I2C_AckConfig(I2C_UNIT, I2C_NACK);
//        }

//        i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_RX, u32Timeout);

//        if (LL_OK == i32Ret) {
//            i32Ret = I2C_MasterReceiveDataAndStop(I2C_UNIT, au8Data, u32Size, u32Timeout);
//        }

//        I2C_AckConfig(I2C_UNIT, I2C_ACK);
//    }

//    if (LL_OK != i32Ret) {
//        (void)I2C_Stop(I2C_UNIT, u32Timeout);
//    }
//    I2C_Cmd(I2C_UNIT, DISABLE);
//    return i32Ret;
//}

void uart1Init(void)
{
	stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;

    GPIO_SetDebugPort(GPIO_PIN_TDI, DISABLE);
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_15,GPIO_FUNC_33);//USART1-RX		串口1 主串口 ：TX:PC10 RX:PA15
    GPIO_SetFunc(GPIO_PORT_C,GPIO_PIN_10,GPIO_FUNC_32);//USART1-TX

	/* Enable USART1 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART1, ENABLE);
    /************************* Configure USART1***************************/
    USART_DeInit(CM_USART1);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART1, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART1, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART1_RI;
    stcIrq.enIRQn = USART1_IRQn;
	stcIrq.pfnCallback = &USART1_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(USART1_IRQn);
	
    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART1_EI;
    stcIrq.enIRQn = INT001_IRQn;
    stcIrq.pfnCallback = &USART1_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT001_IRQn);
    NVIC_SetPriority(INT001_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT001_IRQn);
}

void uart2Init(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;

   	GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_01,GPIO_FUNC_36);//USART2-TX		串口2 甲烷 ：TX:PA01 RX:PA02
    GPIO_SetFunc(GPIO_PORT_A,GPIO_PIN_02,GPIO_FUNC_37);//USART2-RX
    
    /* Enable USART2 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART2, ENABLE);
    /************************* Configure USART2***************************/
    USART_DeInit(CM_USART2);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 115200UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART2, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART2, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART2_RI;
    stcIrq.enIRQn = USART2_IRQn;
	stcIrq.pfnCallback = &USART2_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(USART2_IRQn);
    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART2_EI;
    stcIrq.enIRQn = INT002_IRQn;
    stcIrq.pfnCallback = &USART2_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT002_IRQn);
    NVIC_SetPriority(INT002_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT002_IRQn);
}

void uart3Init(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;
    
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_08,GPIO_FUNC_33);//USART3-RX		串口3 GPS ：TX:PB09 RX:PB08
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_09,GPIO_FUNC_32);//USART3-TX

	/*GPS Enable USART3 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART3, ENABLE);
    /************************* Configure USART3***************************/
    USART_DeInit(CM_USART3);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART3, &stcUartInit, NULL);

    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART3, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART3_RI;
    stcIrq.enIRQn = USART3_IRQn;
	stcIrq.pfnCallback = &USART3_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART3_IRQn);
    NVIC_SetPriority(USART3_IRQn, DDL_IRQ_PRIO_05);
    NVIC_EnableIRQ(USART3_IRQn);
    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART3_EI;
    stcIrq.enIRQn = INT003_IRQn;
    stcIrq.pfnCallback = &USART3_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT003_IRQn);
    NVIC_SetPriority(INT003_IRQn, DDL_IRQ_PRIO_05);
    NVIC_EnableIRQ(INT003_IRQn);
}

void uart4Init(void)
{
	stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;

	GPIO_SetDebugPort(GPIO_PIN_SWO, DISABLE);
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_03,GPIO_FUNC_36);//USART4-TX		串口4 NB ：TX:PB03 RX:PB04  

	GPIO_SetDebugPort(GPIO_PIN_TRST, DISABLE);
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_04,GPIO_FUNC_37);//USART4-RX

	/*  NB Enable USART4 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART4, ENABLE);
    /************************* Configure USART4***************************/
    USART_DeInit(CM_USART4);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART4, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART4, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

	/* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART4_RI;
    stcIrq.enIRQn = USART4_IRQn;
    stcIrq.pfnCallback = &USART4_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART4_IRQn);
    NVIC_SetPriority(USART4_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(USART4_IRQn);
	/* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART4_EI;
    stcIrq.enIRQn = INT004_IRQn;
    stcIrq.pfnCallback = &USART4_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT004_IRQn);
    NVIC_SetPriority(INT004_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT004_IRQn);
}


void uart5Init(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;
    
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_14,GPIO_FUNC_52);//USART5-TX		串口5 H2S ：TX:PB14 RX:PB15
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_15,GPIO_FUNC_53);//USART5-RX

	/* H2S Enable USART5 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART5, ENABLE);
    /************************* Configure USART5***************************/
    USART_DeInit(CM_USART5);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 9600UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART5, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART5, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART5_RI;
    stcIrq.enIRQn = USART5_IRQn;
	stcIrq.pfnCallback = &USART5_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART5_IRQn);
    NVIC_SetPriority(USART5_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(USART5_IRQn);
    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART5_EI;
    stcIrq.enIRQn = INT005_IRQn;
    stcIrq.pfnCallback = &USART5_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT005_IRQn);
    NVIC_SetPriority(INT005_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT005_IRQn);
}

void uart6Init(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrq;

    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_12,GPIO_FUNC_54);//USART6-TX		串口6 雷达 ：TX:PB12 RX:PB13
    GPIO_SetFunc(GPIO_PORT_B,GPIO_PIN_13,GPIO_FUNC_55);//USART6-RX

    /* 雷达测距 Enable USART6 clock */
    FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_USART6, ENABLE);
    /************************* Configure USART6***************************/
    USART_DeInit(CM_USART6);
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockSrc = USART_CLK_SRC_INTERNCLK;
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_DISABLE;
    stcUartInit.u32Baudrate = 115200UL;
    stcUartInit.u32DataWidth = USART_DATA_WIDTH_8BIT;
    stcUartInit.u32StopBit = USART_STOPBIT_1BIT;
    stcUartInit.u32Parity = USART_PARITY_NONE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_16BIT;
    stcUartInit.u32FirstBit = USART_FIRST_BIT_LSB;
    stcUartInit.u32StartBitPolarity = USART_START_BIT_FALLING;
    stcUartInit.u32HWFlowControl = USART_HW_FLOWCTRL_NONE;
    USART_UART_Init(CM_USART6, &stcUartInit, NULL);
    /* Enable USART_TX | USART_RX | USART_INT_RX function */
    USART_FuncCmd(CM_USART6, (USART_TX | USART_RX | USART_INT_RX), ENABLE);

    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART6_RI;
    stcIrq.enIRQn = USART6_IRQn;
	stcIrq.pfnCallback = &USART6_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(USART6_IRQn);
    NVIC_SetPriority(USART6_IRQn, DDL_IRQ_PRIO_10);
    NVIC_EnableIRQ(USART6_IRQn);
    /* IRQ sign-in */
    stcIrq.enIntSrc = INT_SRC_USART6_EI;
    stcIrq.enIRQn = INT006_IRQn;
    stcIrq.pfnCallback = &USART6_Err_Handler;
    (void)INTC_IrqSignIn(&stcIrq);
    /* NVIC config */
    NVIC_ClearPendingIRQ(INT006_IRQn);
    NVIC_SetPriority(INT006_IRQn, DDL_IRQ_PRIO_10);
    NVIC_EnableIRQ(INT006_IRQn);
}

//USARTx Config
void App_USARTxCfg(void)
{
    uart1Init();		//串口1 调试打印
    uart2Init();		//串口2 甲烷 实测4秒左右
    uart3Init();		//串口3 GPS
    uart4Init();		//串口4 NB
    uart5Init();		//串口5 H2S	实测500ms反应时间
    uart6Init();		//串口6 雷达 160ms太快了，改为2s
}

//RTC Config
void App_RTCCfg(void)
{
    stc_rtc_init_t stcRtcInit;
    stc_irq_signin_config_t stcIrq;
    /* Reset RTC counter */
    if (LL_ERR_TIMEOUT != RTC_DeInit()) {
        /* Configure structure initialization */
        (void)RTC_StructInit(&stcRtcInit);

        /* Configuration RTC structure */
        stcRtcInit.u8ClockSrc = RTC_CLK_SRC_XTAL32;
        stcRtcInit.u8HourFormat = RTC_HOUR_FMT_24H;
        stcRtcInit.u8IntPeriod = RTC_INT_PERIOD_PER_SEC;
        stcRtcInit.u8ClockCompen = RTC_CLK_COMPEN_DISABLE;
        (void)RTC_Init(&stcRtcInit);
		    /* IRQ sign-in */
	stcIrq.enIntSrc = INT_SRC_RTC_PRD;
	stcIrq.enIRQn = INT000_IRQn;
	stcIrq.pfnCallback = &RTC_Handler;
	(void)INTC_IrqSignIn(&stcIrq);
        /* Enable RTC interrupt */
        RTC_IntCmd(RTC_INT_PERIOD, ENABLE);
        /* Startup RTC count */
	RTC_Cmd(DISABLE);
    }
	

    /* NVIC config */
    NVIC_ClearPendingIRQ(INT000_IRQn);
    NVIC_SetPriority(INT000_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT000_IRQn);
}

// ============ 初始化RTC（仅执行一次） ============
void App_RTCCfg_LowPower(void)
{
    stc_rtc_init_t stcRtcInit;
    stc_irq_signin_config_t stcIrq;
    stc_rtc_date_t stcRtcDate;
    stc_rtc_time_t stcRtcTime;
//    stc_rtc_alarm_t stcRtcAlarm;
    
    /* 复位RTC */
    if (LL_ERR_TIMEOUT != RTC_DeInit()) {
        
        /* 2. RTC初始化 - 不使用周期中断 */
        (void)RTC_StructInit(&stcRtcInit);
        stcRtcInit.u8ClockSrc = RTC_CLK_SRC_XTAL32; // 外部32.768kHz晶振
        stcRtcInit.u8HourFormat = RTC_HOUR_FMT_24H; // 24小时制
        stcRtcInit.u8IntPeriod = RTC_INT_PERIOD_INVD;  // 不使用秒中断
        stcRtcInit.u8ClockCompen = RTC_CLK_COMPEN_DISABLE;// 关闭补偿
        (void)RTC_Init(&stcRtcInit);
        
        /*设置初始时间0点0分 */
        stcRtcDate.u8Year = 0;
        stcRtcDate.u8Month = RTC_MONTH_JANUARY;
        stcRtcDate.u8Day = 1;
        stcRtcDate.u8Weekday = RTC_WEEKDAY_SUNDAY;
        (void)RTC_SetDate(RTC_DATA_FMT_DEC, &stcRtcDate);
        
        stcRtcTime.u8Hour = 0;
        stcRtcTime.u8Minute = 0;
        stcRtcTime.u8Second = 0;
        stcRtcTime.u8AmPm = RTC_HOUR_24H;
        (void)RTC_SetTime(RTC_DATA_FMT_DEC, &stcRtcTime);

        /*中断注册 - 闹钟中断 */
        stcIrq.enIntSrc = INT_SRC_RTC_ALM;
        stcIrq.enIRQn = INT000_IRQn;
        stcIrq.pfnCallback = &RTC_AlarmHandler;
        (void)INTC_IrqSignIn(&stcIrq);
        
        /* 使能闹钟 */
        RTC_AlarmCmd(DISABLE);

        
        /* 7. 使能闹钟中断 */
        RTC_IntCmd(RTC_INT_ALARM, ENABLE);
        
        /* 8. 启动RTC */
        RTC_Cmd(DISABLE);
    }
    
    /* 9. NVIC配置 */
    NVIC_ClearPendingIRQ(INT000_IRQn);
    NVIC_SetPriority(INT000_IRQn, DDL_IRQ_PRIO_15);
    NVIC_EnableIRQ(INT000_IRQn);
}

// ============ 设置闹钟（每次唤醒后调用） ============
void App_RTC_SetAlarm(uint16_t u16Minutes)
{
    stc_rtc_alarm_t stcRtcAlarm;
	stc_rtc_alarm_t rb;
    uint8_t u8Hour, u8Minute;

	RTC_Cmd(DISABLE);
	RTC_AlarmCmd(DISABLE);

    App_RTC_ResetTime();
    
    /* 计算闹钟时间（从0点开始） */
    u8Hour = u16Minutes / 60;
    u8Minute = u16Minutes % 60;
   
	 /* 进入临界区：关闭全局中断，防止握手被中断打断 */
    __disable_irq();  

	/* 进读写模式 */
	if (LL_OK != RTC_EnterRwMode()) {
		printf("[RTC-RESET] EnterRwMode failed\n");
	}

    /* 配置闹钟 */
    stcRtcAlarm.u8AlarmHour = u8Hour;
    stcRtcAlarm.u8AlarmMinute = u8Minute;
    stcRtcAlarm.u8AlarmWeekday = RTC_ALARM_WEEKDAY_EVERYDAY;  // 每天匹配
    stcRtcAlarm.u8AlarmAmPm = RTC_HOUR_24H;
    
    (void)RTC_SetAlarm(RTC_DATA_FMT_DEC, &stcRtcAlarm);

	/* 回读验证 */
	RTC_GetAlarm(RTC_DATA_FMT_DEC, &rb);
	printf("[SET-ALARM] readback: %02d:%02d, weekday=0x%02X, ampm=%d\n",
       rb.u8AlarmHour, rb.u8AlarmMinute, rb.u8AlarmWeekday, rb.u8AlarmAmPm);

	/* 退出读写模式 */
    (void)RTC_ExitRwMode();

	/* 退出临界区 */
    __enable_irq(); 
    
    /* 使能闹钟 */
    RTC_AlarmCmd(ENABLE);

    /* 启动RTC */
    RTC_Cmd(ENABLE);
}

// ============ 重置RTC时间为0点 ============
void App_RTC_ResetTime(void)
{
    stc_rtc_time_t stcRtcTime;
    stc_rtc_time_t stcReadBack;
    int32_t ret;

    /* 进读写模式 */
    if (LL_OK != RTC_EnterRwMode()) {
        printf("[RTC-RESET] EnterRwMode failed\n");
    }
    
    stcRtcTime.u8Hour = 0;
    stcRtcTime.u8Minute = 0;
    stcRtcTime.u8Second = 0;
    stcRtcTime.u8AmPm = RTC_HOUR_24H;
    
    ret = RTC_SetTime(RTC_DATA_FMT_DEC, &stcRtcTime);

    /* 退出读写模式 */
    (void)RTC_ExitRwMode();

    /* 回读验证 */
    (void)RTC_GetTime(RTC_DATA_FMT_DEC, &stcReadBack);

    printf("[RTC-RESET] ret=%d, readback=%02d:%02d:%02d\n",
           ret,
           stcReadBack.u8Hour,
           stcReadBack.u8Minute,
           stcReadBack.u8Second);
}

/**
 * @brief  MCU behavior config for stop mode.
 * @param  None
 * @retval None
 */
void STOP_Config(void)
{
    stc_pwc_stop_mode_config_t stcStopConfig;

    (void)PWC_STOP_StructInit(&stcStopConfig);
	
	INTC_WakeupSrcCmd(INTC_STOP_WKUP_RTC_ALM, DISABLE);
    INTC_WakeupSrcCmd(INTC_STOP_WKUP_EXTINT_CH11, DISABLE);
    INTC_WakeupSrcCmd(INTC_STOP_WKUP_EXTINT_CH12, DISABLE);

    stcStopConfig.u16ExBusHold = PWC_STOP_EXBUS_HIZ;
    stcStopConfig.u16Clock = PWC_STOP_CLK_KEEP;
    stcStopConfig.u16FlashWait = PWC_STOP_FLASH_WAIT_ON;
	stcStopConfig.u8StopDrv = PWC_STOP_DRV_LOW;
    (void)PWC_STOP_Config(&stcStopConfig);

    /* Wake-up source config (EXTINT Ch.6 here) */
    INTC_WakeupSrcCmd(INTC_STOP_WKUP_EXTINT_CH12, ENABLE);
    INTC_WakeupSrcCmd(INTC_STOP_WKUP_RTC_ALM, ENABLE);
    INTC_WakeupSrcCmd(INTC_STOP_WKUP_EXTINT_CH11, ENABLE);//Q_CHECK
}

