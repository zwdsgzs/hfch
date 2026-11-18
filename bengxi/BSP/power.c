#include "config.h"

void Interrupt_Off(void)
{
	LL_PERIPH_WE(LL_PERIPH_ALL);
	USART_FuncCmd(CM_USART1, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
	USART_FuncCmd(CM_USART2, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
	USART_FuncCmd(CM_USART3, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
	USART_FuncCmd(CM_USART4, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
	USART_FuncCmd(CM_USART5, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
	USART_FuncCmd(CM_USART6, (USART_TX | USART_RX | USART_INT_RX), DISABLE);
    LL_PERIPH_WP(LL_PERIPH_ALL);
}

void Interrupt_ON(void)
{
    LL_PERIPH_WE(LL_PERIPH_ALL);
	USART_FuncCmd(CM_USART1, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	USART_FuncCmd(CM_USART2, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	USART_FuncCmd(CM_USART3, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	USART_FuncCmd(CM_USART4, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	USART_FuncCmd(CM_USART5, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	USART_FuncCmd(CM_USART6, (USART_TX | USART_RX | USART_INT_RX), ENABLE);
	
	App_ADCCfg();
LL_PERIPH_WP(LL_PERIPH_ALL);
}

void NbIot_PowerOn(void)
{

  GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_06);// NB_En
	GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_05);// NB_Wake_up  
	GPIO_SetPins(GPIO_PORT_D, GPIO_PIN_02);// NB_Rst
}
void NbIot_PowerOff(void)
{

  GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_06);// NB_En
	GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_05);// NB_Wake_up  
	GPIO_ResetPins(GPIO_PORT_D, GPIO_PIN_02);// NB_Rst
}


void EER_PowerOn(void)
{
	GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_01);// EER_En
	GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_00);// EE_SCL  
	GPIO_SetPins(GPIO_PORT_C, GPIO_PIN_05);// EE_SDA
}
void EER_PowerOff(void)
{
	GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_01);// EER_En
	GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_00);// EE_SCL  
	GPIO_ResetPins(GPIO_PORT_C, GPIO_PIN_05);// EE_SDA
}

void Sensor_PowerOn(void)
{
		GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_06);// NB_En
		GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_05);// NB_Wake_up  
		GPIO_SetPins(GPIO_PORT_D, GPIO_PIN_02);// NB_Rst

		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_00);// CH4_En

		GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_07);// GPS_En	

		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_07);// MOTOR_En	

		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_05);// WF_En(Air Press)

		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_06);// Set +3V6_EN
		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_07);// Set MOTOR_EN
		GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_02);// Set Sensor_H2S_radar_En
		GPIO_SetPins(GPIO_PORT_H, GPIO_PIN_02);// Set +5V_EN
		GPIO_SetPins(GPIO_PORT_C, GPIO_PIN_13);// Set ADC_Ref
}

void Sensor_PowerOff(void)
{	
		GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_06);// NB_En
		GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_05);// NB_Wake_up  
		GPIO_ResetPins(GPIO_PORT_D, GPIO_PIN_02);// NB_Rst

		GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_00);// CH4_En

		GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_07);// GPS_En	

		GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_07);// MOTOR_En	

		GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_05);// WF_En(Air Press)

		GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_06);// Set +3V6_EN
		GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_07);// Set MOTOR_EN
		GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_02);// Set Sensor_H2S_radar_En
		GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_02);// Set +5V_EN
		//GPIO_ResetPins(GPIO_PORT_C, GPIO_PIN_13);// Set ADC_Ref
}

/*

低功耗GPIO配置原则： 
	外部有上拉的,配置成输出高；
	外部有下拉的，配置成输出低；
	悬空的管脚，统一配置成下拉输入。
	没有上拉下拉的输出管脚配置成输出低
*/
void sleep_pin_Power_cfg(void)
{
	PA3_REF_SET;
	V5_RESET;
	V3_6_RESET;
	GPS_POWER_RESET;
	H2S_POWER_RESET;
	CH4_POWER_RESET;
	RADA_POWER_RESET;
	MOTOR_POWER_RESET;
	EER_PowerOff();
	NbIot_PowerOff();
	LED_RESET;//led
	
	Interrupt_Off();	
	FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_ADC1, DISABLE);
	ADC_ChCmd(CM_ADC1, ADC_SEQ_A, ADC_CH4|ADC_CH10|ADC_CH11, DISABLE);
	ADC_Stop(CM_ADC1);
}

void wkp_pin_Power_cfg(void)
{
	PA3_REF_SET;
	V5_SET;
	V3_6_SET;
	GPS_POWER_SET;
	H2S_POWER_SET;
	CH4_POWER_SET;
	RADA_POWER_SET;
	MOTOR_POWER_SET;
	EER_PowerOn();
	NbIot_PowerOn();
	
	Interrupt_Off();	
	FCG_Fcg3PeriphClockCmd(FCG3_PERIPH_ADC1, DISABLE);
	ADC_ChCmd(CM_ADC1, ADC_SEQ_A, ADC_CH4|ADC_CH10|ADC_CH11, DISABLE);
	ADC_Stop(CM_ADC1);
}

void Sleep()
{
	get_frist_Mq_flg = 0;	
	opentop = false;

	sleep_pin_Power_cfg();		
	SysTick_Suspend();	
	LL_PERIPH_WE(LL_PERIPH_ALL);
	PWC_STOP_Enter(PWC_STOP_WFI);
	LL_PERIPH_WP(LL_PERIPH_ALL);
}

void Wake(void)
{
	SysTick_Resume();
	Interrupt_ON();			
	/* Clear flag */
	EXTINT_ClearExtIntStatus(KEY_EXTINT_CH);
	/* Clear pending */
	NVIC_ClearPendingIRQ(KEY_INT_IRQn);
	/* Clear pending */
	NVIC_ClearPendingIRQ(RTC_IRQn);
	/* Clear flag */
	EXTINT_ClearExtIntStatus(Q_CHECK_EXTINT_CH);
	/* Clear pending */
	NVIC_ClearPendingIRQ(Q_CHECK_INT_IRQn);
	
	

	RTC_Cmd(DISABLE);
	RTC_AlarmCmd(DISABLE);

	NbIot_PowerOn();
	DDL_DelayMS(3000);
}

