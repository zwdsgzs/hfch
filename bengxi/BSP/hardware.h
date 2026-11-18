#ifndef __HARDWARE_H
#define __HARDWARE_H

#include "main.h"

extern uint8_t Factory_mode;
extern uint8_t MOTOR_START;
//extern uint8_t MOTOR_START,CH4_START,sleep_time,Factory_mode;
extern uint8_t CH4_timeout,H2S_timeout,RADAR_timeout,o2_co_timeout,RS485_timeout;
extern uint8_t MOTOR_flag;
//extern uint16_t RTC_Time_MOTOR_Stamp;

#define KEY_PORT               (GPIO_PORT_A)
#define KEY_PIN                (GPIO_PIN_12)
#define KEY_EXTINT_CH          (EXTINT_CH12)
#define KEY_INT_SRC            (INT_SRC_PORT_EIRQ12)
#define KEY_INT_IRQn           (INT013_IRQn)
#define KEY_INT                (INTC_INT13)
#define KEY_EVT                (INTC_EVT13)


#define Q_CHECK_PORT               (GPIO_PORT_A)
#define Q_CHECK_PIN                (GPIO_PIN_11)
#define Q_CHECK_EXTINT_CH          (EXTINT_CH11)
#define Q_CHECK_INT_SRC            (INT_SRC_PORT_EIRQ11)
#define Q_CHECK_INT_IRQn           (INT014_IRQn)
#define Q_CHECK_INT                (INTC_INT14)
#define Q_CHECK_EVT                (INTC_EVT14)

void App_ClkCfg(void);

void App_PortCfg(void);
void App_FreePortCfg(void);

void App_Port_WKP(void);

void TMR0_Config(void);

void App_ADCCfg(void);

void IIC_PortCfg(void);
void App_I2CxCfg(void);
int32_t I2C_Master_Transmit(uint16_t u16DevAddr, const uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout);
int32_t I2C_Master_Receive(uint16_t u16DevAddr, uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout);

void App_USARTxCfg(void);

void App_RTCCfg(void);

void App_RTCCfg_LowPower(void);
void App_RTC_SetAlarm(uint16_t u16Minutes);
void App_RTC_ResetTime(void);

void STOP_Config(void);

#endif 

