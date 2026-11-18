/*lic*/
#ifndef __MAIN_H__
#define __MAIN_H__

#include "hc32_ll.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"


extern bool timFlag;
extern bool uart3errflag;
//extern int timecnt;
extern bool start;
extern bool key_locked;
// 状态类型定义
typedef enum {
    STATE_RUNNING,
    STATE_STOPPED,
    STATE_CONFIG,
} State_t;

// 状态机结构
typedef struct {
    State_t state;
    int counter;  // 运行时的计数器
}StateMachine;

//流程
#define T_COUNT       50
#define T_CONNECT     40
#define T_SENSOR_ON   34
#define T_MOTOR_OFF   20
#define T_H2S_0       15
#define T_RADAR_0     14
#define T_H2S_1       13
#define T_RADAR_1     12
#define T_ADC         11
#define T_H2S_2       10
#define T_RADAR_2     9 
#define T_H2S_3       8 
#define T_RADAR_3     7 
#define T_H2S_4       6 
#define T_RADAR_4     5
#define T_PUB         4
#define T_SUB         2

//------------------唤醒-----------------
#define GET_Q_CHECK    GPIO_ReadInputPins(GPIO_PORT_A, GPIO_PIN_11)//倾倒
#define GET_KEY_WKUP   GPIO_ReadInputPins(GPIO_PORT_A, GPIO_PIN_12)//磁开关

//------------------传感器-----------------
#define CH4_POWER_SET   GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_00)
#define CH4_POWER_RESET   GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_00)

#define H2S_POWER_SET   GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_02)
#define H2S_POWER_RESET   GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_02)

#define GPS_POWER_SET     GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_07)
#define GPS_POWER_RESET   GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_07)

#define RADA_POWER_SET     GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_04)
#define RADA_POWER_RESET   GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_04)

//------------------电池ADC采集-------------
#define PA3_REF_SET		GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_03);
#define PA3_REF_RESET	GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_03);

//--------------EEROM-------------
#define EEROM_EN_POWER_SET   GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_01)
#define EEROM_EN_POWER_RESET   GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_01)

//--------------NB-------------
#define NB_EN_POWER_SET   GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_06)
#define NB_EN_POWER_RESET   GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_06)

//-------------------电源控制-------------------
#define V5_SET     GPIO_SetPins(GPIO_PORT_H, GPIO_PIN_02)
#define V5_RESET   GPIO_ResetPins(GPIO_PORT_H, GPIO_PIN_02)

#define V3_6_SET     GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_06)
#define V3_6_RESET   GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_06)

//-------------------电机控制-------------------
#define MOTOR_POWER_SET     GPIO_SetPins(GPIO_PORT_A, GPIO_PIN_07)//气泵电源控制
#define MOTOR_POWER_RESET   GPIO_ResetPins(GPIO_PORT_A, GPIO_PIN_07)
#define MOTOR_POWER_Toggle   GPIO_TogglePins(GPIO_PORT_A, GPIO_PIN_07)

//-------------------LED-------------------
#define LED_SET   GPIO_SetPins(GPIO_PORT_C, GPIO_PIN_04)
#define LED_RESET   GPIO_SetPins(GPIO_PORT_C, GPIO_PIN_04)
#define LED_Toggle   GPIO_TogglePins(GPIO_PORT_C, GPIO_PIN_04)

//#define RS485_SET 		GPIO_SetPins(GPIO_PORT_B, GPIO_PIN_07);
//#define RS485_RESET		GPIO_ResetPins(GPIO_PORT_B, GPIO_PIN_07);

/* Define I2C unit used for the example */
#define I2C_UNIT                        (CM_I2C1)
#define I2C_FCG                         (FCG1_PERIPH_I2C1)


/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT                    (GPIO_PORT_B)
#define I2C_SCL_PIN                     (GPIO_PIN_03)
#define I2C_SDA_PORT                    (GPIO_PORT_B)
#define I2C_SDA_PIN                     (GPIO_PIN_04)
#define I2C_GPIO_SCL_FUNC               (GPIO_FUNC_49)
#define I2C_GPIO_SDA_FUNC               (GPIO_FUNC_48)
 
#define TIMEOUT                         (0x40000UL)

/* Define Write and read data length for the example */
#define TEST_DATA_LEN                   (256U)
/* Define i2c baudrate */
#define I2C_BAUDRATE                    (100000UL)



/* TMR0 unit and channel definition */
#define TMR0_UNIT                       (CM_TMR0_1)
#define TMR0_CLK                        (FCG2_PERIPH_TMR0_1)
#define TMR0_CH                         (TMR0_CH_B)
#define TMR0_TRIG_CH                    (AOS_TMR0)
#define TMR0_CH_INT                     (TMR0_INT_CMP_B)
#define TMR0_CH_FLAG                    (TMR0_FLAG_CMP_B)
#define TMR0_INT_SRC                    (INT_SRC_TMR0_1_CMP_B)
#define TMR0_IRQn                       (INT008_IRQn)
/* Period = 1 / (Clock freq / div) * (Compare value + 1) = 500ms */
#define TMR0_CMP_VALUE                  (XTAL_VALUE / 16U / 2U - 1U)

uint32_t User_Time_Read(uint32_t time);
void User_Time_Set(uint32_t* time);


void KEY_IrqHandle(void);
void Q_CHECK_IrqHandle(void);
void USART1_Handler(void);
void USART2_Handler(void);
void USART3_Handler(void);
void USART4_Handler(void);
void USART5_Handler(void);
void USART6_Handler(void);

/* INT_SRC_USART5_EI Callback. */
void USART5_Err_Handler(void);
/* INT_SRC_USART2_EI Callback. */
void USART2_Err_Handler(void);
/* INT_SRC_USART6_EI Callback. */
void USART6_Err_Handler(void);
/* INT_SRC_USART4_EI Callback. */
void USART4_Err_Handler(void);
/* INT_SRC_USART1_EI Callback. */
void USART1_Err_Handler(void);
/* INT_SRC_USART3_EI Callback. */
void USART3_Err_Handler(void);
/* INT_SRC_RTC_PRD Callback. */
void RTC_Handler(void);

void RTC_AlarmHandler(void);

void TMR0_CompareIrqCallback(void);

static void BufWrite(uint8_t au8Buf[], uint8_t u8Data);
static uint8_t BufRead(uint8_t au8Buf[]);
static void Master_Start(void);
void Master_SendAddr(uint8_t u8Addr, uint8_t u8Dir);

void run_board_funtion (void);
void Fun_motor_power_supply(void);
void Workjob(void);

void timerOutAction(void);

#endif /* __MAIN_H__ */

/*eof*/
