#include "HDC2010.h"

int HDC2010ReadReg(uint8_t reg, uint8_t *data)
{
    if (I2C_Read_Byte(HDC2010_ADD, reg, data) != HAL_OK) 
	{
        return -1;
    }
    return 0;
}

int HDC2010WriteReg(uint8_t reg, uint8_t data)
{
    if (I2C_Write_Byte(HDC2010_ADD, reg, data) != HAL_OK) 
	{
        return -1;
    }
    return 0;
}

int HDC2010ReadMulti(uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (I2C_Read_Buffer(HDC2010_ADD, reg, buf, len) != HAL_OK) 
	{
        return -1;
    }
    return 0;
}

/* ============================================================
 *                    读温湿度 / 器件 ID
 * ============================================================ */
float HDC2010ReadTemp(void)
{
    uint8_t buf[2];
	float fTemp = 0.0;
    if (HDC2010ReadMulti(HDC2010_TEMP_LOW, buf, 2) != 0) {
        return -999.0f;
    }
    uint16_t raw = ((uint16_t)buf[1] << 8) | buf[0];
	fTemp = ((float)raw * 165.0f / 65536.0f) - 40.0f;

	stu_sensor.sTemp[stu_sensor.collectCnt] = (unsigned short)(fTemp * 10);
    return fTemp;
}

float HDC2010ReadHumidity(void)
{
    uint8_t buf[2];
	float fHum = 0.0;
    if (HDC2010ReadMulti(HDC2010_HUMID_LOW, buf, 2) != 0) {
        return -999.0f;
    }
    uint16_t raw = ((uint16_t)buf[1] << 8) | buf[0];
	fHum = ((float)raw * 100.0f / 65536.0f);

	stu_sensor.sHum[stu_sensor.collectCnt] = (unsigned short)(fHum * 10);
    return fHum;
}

int HDC2010ReadDeviceID(uint16_t *id)
{
    uint8_t buf[2];
    if (HDC2010ReadMulti(HDC2010_DEVICE_ID_L, buf, 2) != 0) {
        return -1;
    }
    *id = ((uint16_t)buf[1] << 8) | buf[0];
    return 0;
}

/* ============================================================
 *                    配置函数
 * ============================================================ */
void HDC2010Reset(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    cfg |= 0x80;   /* bit7 = SOFT_RES */
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
    HAL_Delay(10);
}

void HDC2010TriggerMeasurement(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_MEASUREMENT_CONFIG, &cfg) != 0) return;
    cfg |= 0x01;   /* bit0 = MEAS_TRIG */
    HDC2010WriteReg(HDC2010_MEASUREMENT_CONFIG, cfg);
}

void HDC2010SetTempRes(uint8_t res)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_MEASUREMENT_CONFIG, &cfg) != 0) return;
    cfg &= ~(0x03 << 6);           /* 清除 TRES[1:0] */
    cfg |=  ((res & 0x03) << 6);
    HDC2010WriteReg(HDC2010_MEASUREMENT_CONFIG, cfg);
}

void HDC2010SetHumidRes(uint8_t res)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_MEASUREMENT_CONFIG, &cfg) != 0) return;
    cfg &= ~(0x03 << 4);           /* 清除 HRES[1:0] */
    cfg |=  ((res & 0x03) << 4);
    HDC2010WriteReg(HDC2010_MEASUREMENT_CONFIG, cfg);
}

void HDC2010SetMeasurementMode(uint8_t mode)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_MEASUREMENT_CONFIG, &cfg) != 0) return;
    cfg &= ~(0x03 << 1);           /* 清除 MEAS_CONF[1:0] */
    cfg |=  ((mode & 0x03) << 1);
    HDC2010WriteReg(HDC2010_MEASUREMENT_CONFIG, cfg);
}

void HDC2010SetRate(uint8_t rate)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    cfg &= ~(0x07 << 4);           /* 清除 AMM[2:0] */
    cfg |=  ((rate & 0x07) << 4);
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
}

void HDC2010EnableDRDYInterrupt(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_INTERRUPT_CONFIG, &cfg) != 0) return;
    cfg |= 0x80;   /* bit7 = DRDY_ENABLE */
    HDC2010WriteReg(HDC2010_INTERRUPT_CONFIG, cfg);
}

void HDC2010DisableDRDYInterrupt(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_INTERRUPT_CONFIG, &cfg) != 0) return;
    cfg &= ~0x80;
    HDC2010WriteReg(HDC2010_INTERRUPT_CONFIG, cfg);
}

void HDC2010EnableDRDY_Pin(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    cfg |= (1 << 2);    /* DRDY/INT_EN = 1 */
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
}

void HDC2010DisableDRDY_Pin(void)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    cfg &= ~(1 << 2);
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
}

void HDC2010SetInterruptPolarity(uint8_t pol)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    if (pol == HDC2010_INT_ACTIVE_HIGH) {
        cfg |= (1 << 1);
    } else {
        cfg &= ~(1 << 1);
    }
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
}

void HDC2010SetInterruptMode(uint8_t mode)
{
    uint8_t cfg;
    if (HDC2010ReadReg(HDC2010_CONFIG, &cfg) != 0) return;
    if (mode == HDC2010_INT_COMPARATOR) {
        cfg |= 0x01;
    } else {
        cfg &= ~0x01;
    }
    HDC2010WriteReg(HDC2010_CONFIG, cfg);
}

/* ============================================================
 *                    初始化
 * ============================================================ */
/**
 * @brief  HDC2010 完整初始化
 * @note   调用前请确保 MX_I2C1_Init() 已完成
 */
void HDC2010_Init(void)
{
    /* 软复位 */
    HDC2010Reset();
    HAL_Delay(10);

    /* 校验器件 ID（应为 0x07D0） */
    uint16_t id = 0;
    if (HDC2010ReadDeviceID(&id) != 0 || id != 0x07D0) {
        /* 通信失败或器件型号不对，可在此加错误处理 */
        return;
    }

    /* 设置分辨率（默认 14 位即可） */
    HDC2010SetTempRes(HDC2010_RES_14BIT);
    HDC2010SetHumidRes(HDC2010_RES_14BIT);

    /* 测量模式：温度 + 湿度 */
    HDC2010SetMeasurementMode(HDC2010_MEAS_TEMP_HUMID);

    /* 手动模式（按需触发） */
    HDC2010SetRate(HDC2010_RATE_MANUAL);

    /* 配置 DRDY 中断 */
    HDC2010SetInterruptPolarity(HDC2010_INT_ACTIVE_LOW);   /* 低有效 */
    HDC2010SetInterruptMode(HDC2010_INT_LEVEL);            /* 电平敏感 */
    HDC2010EnableDRDYInterrupt();                           /* 使能中断发生器 */
    HDC2010EnableDRDY_Pin();                                /* 使能引脚输出 */

    /* 触发第一次测量 */
//    HDC2010TriggerMeasurement();
}



