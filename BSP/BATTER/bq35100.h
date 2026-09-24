#ifndef __BQ35100_H
#define __BQ35100_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ============================================================
 *                      硬件配置
 * ============================================================ */

/* BQ35100 7 位 I2C 地址：固定为 1010101 = 0x55 */
#define BQ35100_ADDR_7BIT           0x55

/* ============================================================
 *                      命令地址
 * ============================================================ */

#define BQ35100_CMD_CONTROL              0x00    /* Control() */
#define BQ35100_CMD_ACCUMULATED_CAPACITY 0x02    /* AccumulatedCapacity() */
#define BQ35100_CMD_TEMPERATURE          0x06    /* Temperature() */
#define BQ35100_CMD_VOLTAGE              0x08    /* Voltage() */
#define BQ35100_CMD_BATTERY_STATUS       0x0A    /* BatteryStatus() */
#define BQ35100_CMD_BATTERY_ALERT        0x0B    /* BatteryAlert() */
#define BQ35100_CMD_CURRENT              0x0C    /* Current() */
#define BQ35100_CMD_STATE_OF_HEALTH      0x2E    /* StateOfHealth() */

/* ============================================================
 *                      Control() 子命令
 * ============================================================ */

#define BQ35100_CTRL_GAUGE_START        0x0011
#define BQ35100_CTRL_GAUGE_STOP         0x0012
#define BQ35100_CTRL_NEW_BATTERY        0xA613

/* ============================================================
 *                      数据结构
 * ============================================================ */

typedef struct {
    uint32_t accumulated_capacity_uAh;   /* 累计放电量（?Ah） */
    uint16_t voltage_mV;                 /* 电池电压（mV） */
    int16_t  current_mA;                 /* 电流（mA） */
    float    temperature_C;              /* 温度（℃） */
    uint8_t  state_of_health;            /* SOH（%） */
    uint8_t  battery_status;             /* BatteryStatus 原始值 */
    uint8_t  battery_alert;              /* BatteryAlert 原始值 */

    /* 错误码：0 = 成功，非 0 = 失败 */
    uint8_t  error_capacity;
    uint8_t  error_voltage;
    uint8_t  error_current;
    uint8_t  error_temperature;
    uint8_t  error_soh;
    uint8_t  error_status;
} bq35100_t;

extern bq35100_t bq35100;

/* ============================================================
 *                      API
 * ============================================================ */

/**
 * @brief  BQ35100 初始化
 * @note   调用前请确保 MX_I2C1_Init() 已完成
 *         需要 GE 引脚已配置为推挽输出
 */
void BQ35100_Init(void);

/**
 * @brief  等待初始化完成（INITCOMP = 1，ALERT 变低）
 * @param  timeout_ms  超时时间（ms）
 * @retval 0 成功，-1 超时
 */
int BQ35100_WaitInitComplete(uint32_t timeout_ms);

/**
 * @brief  发送 Control() 子命令
 * @param  subcmd  子命令（16 位）
 * @retval 0 成功，-1 失败
 */
int BQ35100_SendControl(uint16_t subcmd);

/**
 * @brief  发送 GAUGE_START，开始累积放电量
 */
int BQ35100_GaugeStart(void);

/**
 * @brief  发送 GAUGE_STOP，停止累积并保存数据
 */
int BQ35100_GaugeStop(void);

/**
 * @brief  读取累计放电量（?Ah）
 * @note   这是“用了多少电”的核心数据
 */
int BQ35100_ReadAccumulatedCapacity(void);

/**
 * @brief  读取电压（mV）
 */
int BQ35100_ReadVoltage(void);

/**
 * @brief  读取电流（mA）
 */
int BQ35100_ReadCurrent(void);

/**
 * @brief  读取温度（℃）
 */
int BQ35100_ReadTemperature(void);

/**
 * @brief  读取 SOH（%）
 */
int BQ35100_ReadStateOfHealth(void);

/**
 * @brief  读取 BatteryStatus 和 BatteryAlert
 */
int BQ35100_ReadStatus(void);

/**
 * @brief  一次性读取所有数据
 */
void BQ35100_ReadAll(void);

#ifdef __cplusplus
}

#endif


#endif















