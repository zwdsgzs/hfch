#include "bq35100.h"

/* 全局数据结构 */
bq35100_t bq35100;

/* ============================================================
 *                  底层读写封装
 * ============================================================ */

static int bq35100_read(uint8_t cmd, uint8_t *buf, uint16_t len)
{
    if (I2C_Read_Buffer(BQ35100_ADDR_7BIT, cmd, buf, len) != HAL_OK) {
        return -1;
    }
    return 0;
}

static int bq35100_write(uint8_t cmd, uint8_t *buf, uint16_t len)
{
    if (I2C_Write_Buffer(BQ35100_ADDR_7BIT, cmd, buf, len) != HAL_OK) {
        return -1;
    }
    return 0;
}

/* ============================================================
 *                  Control() 命令
 * ============================================================ */

int BQ35100_SendControl(uint16_t subcmd)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(subcmd & 0xFF);         /* 低字节先发 */
    buf[1] = (uint8_t)((subcmd >> 8) & 0xFF);  /* 高字节后发 */

    if (bq35100_write(BQ35100_CMD_CONTROL, buf, 2) != 0) {
        return -1;
    }
    return 0;
}

int BQ35100_GaugeStart(void)
{
    return BQ35100_SendControl(BQ35100_CTRL_GAUGE_START);
}

int BQ35100_GaugeStop(void)
{
    return BQ35100_SendControl(BQ35100_CTRL_GAUGE_STOP);
}

/* ============================================================
 *                  等待初始化完成
 * ============================================================ */

int BQ35100_WaitInitComplete(uint32_t timeout_ms)
{
    uint32_t tick_start = HAL_GetTick();

    while ((HAL_GetTick() - tick_start) < timeout_ms) {
        /* 读 BatteryStatus，检查 INITCOMP 位
         * BatteryStatus() bit7 = INITCOMP
         */
        uint8_t status = 0;
        if (bq35100_read(BQ35100_CMD_BATTERY_STATUS, &status, 1) == 0) {
            if (status & 0x80) {   /* INITCOMP = 1 */
                return 0;
            }
        }
        HAL_Delay(10);
    }
    return -1;   /* 超时 */
}

/* ============================================================
 *                  数据读取
 * ============================================================ */

int BQ35100_ReadAccumulatedCapacity(void)
{
    uint8_t buf[4];

    if (bq35100_read(BQ35100_CMD_ACCUMULATED_CAPACITY, buf, 4) != 0) {
        bq35100.error_capacity = 1;
        return -1;
    }

    /* AccumulatedCapacity() 是 32 位无符号整数，小端序 */
    bq35100.accumulated_capacity_uAh =
        ((uint32_t)buf[3] << 24) |
        ((uint32_t)buf[2] << 16) |
        ((uint32_t)buf[1] << 8)  |
        ((uint32_t)buf[0]);

	stu_sensor.unBatEle = bq35100.accumulated_capacity_uAh;

	stu_sensor.uchBattery = 100 - (unsigned char)((stu_sensor.unBatEle / 19000000) * 100);

    bq35100.error_capacity = 0;
    return 0;
}

int BQ35100_ReadVoltage(void)
{
    uint8_t buf[2];

    if (bq35100_read(BQ35100_CMD_VOLTAGE, buf, 2) != 0) {
        bq35100.error_voltage = 1;
        return -1;
    }

    bq35100.voltage_mV = ((uint16_t)buf[1] << 8) | buf[0];
    bq35100.error_voltage = 0;
    return 0;
}

int BQ35100_ReadCurrent(void)
{
    uint8_t buf[2];

    if (bq35100_read(BQ35100_CMD_CURRENT, buf, 2) != 0) {
        bq35100.error_current = 1;
        return -1;
    }

    bq35100.current_mA = (int16_t)(((uint16_t)buf[1] << 8) | buf[0]);
    bq35100.error_current = 0;
    return 0;
}

int BQ35100_ReadTemperature(void)
{
    uint8_t buf[2];
    int16_t raw;

    if (bq35100_read(BQ35100_CMD_TEMPERATURE, buf, 2) != 0) {
        bq35100.error_temperature = 1;
        return -1;
    }

    raw = (int16_t)(((uint16_t)buf[1] << 8) | buf[0]);
    /* 数据手册：Temperature() 单位 0.1 K，0.1K = 0.1℃ */
    bq35100.temperature_C = ((float)raw / 10.0f) - 273.15f;
    bq35100.error_temperature = 0;
    return 0;
}

int BQ35100_ReadStateOfHealth(void)
{
    uint8_t buf = 0;

    if (bq35100_read(BQ35100_CMD_STATE_OF_HEALTH, &buf, 1) != 0) {
        bq35100.error_soh = 1;
        return -1;
    }

    bq35100.state_of_health = buf;
    bq35100.error_soh = 0;
    return 0;
}

int BQ35100_ReadStatus(void)
{
    uint8_t status = 0;
    uint8_t alert = 0;

    if (bq35100_read(BQ35100_CMD_BATTERY_STATUS, &status, 1) != 0) {
        bq35100.error_status = 1;
        return -1;
    }

    if (bq35100_read(BQ35100_CMD_BATTERY_ALERT, &alert, 1) != 0) {
        bq35100.error_status = 1;
        return -1;
    }

    bq35100.battery_status = status;
    bq35100.battery_alert  = alert;
    bq35100.error_status = 0;
    return 0;
}

void BQ35100_ReadAll(void)
{
    BQ35100_ReadVoltage();
    BQ35100_ReadCurrent();
    BQ35100_ReadTemperature();
    BQ35100_ReadStateOfHealth();
    BQ35100_ReadAccumulatedCapacity();
    BQ35100_ReadStatus();
}

/* ============================================================
 *                  初始化
 * ============================================================ */

void BQ35100_Init(void)
{
    /* 1. 给 BQ35100 上电 */
    GE_SET;
    HAL_Delay(10);

    /* 3. 等待 INITCOMP = 1（初始化完成） */
    if (BQ35100_WaitInitComplete(500) != 0) {
        /* 初始化超时，可在此加错误处理 */
        return;
    }

    /* 4. 清除 ALERT（读 BatteryStatus 会清） */
    BQ35100_ReadStatus();

    /* 5. 发送 GAUGE_START，开始累积放电量 */
    BQ35100_GaugeStart();
}










