#include "IIS2DLP.h"

/* ============================================================
 *                  底层 I2C 读写封装
 * ============================================================ */

static int iis2dlpc_read_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
    if (I2C_Read_Buffer(IIS2DLP_ADD, reg, data, len) != HAL_OK) {
        return -1;
    }
    return 0;
}

static int iis2dlpc_write_reg(uint8_t reg, uint8_t *data, uint16_t len)
{
    if (I2C_Write_Buffer(IIS2DLP_ADD, reg, data, len) != HAL_OK) {
        return -1;
    }
    return 0;
}

/* ============================================================
 *                  CTRL1 (0x20) 配置
 * ============================================================ */

/* CTRL1 位域：
 *   bit[7:4] ODR[3:0]
 *   bit[3:2] MODE[1:0]
 *   bit[1:0] LP_MODE[1:0]
 */
static int iis2dlpc_set_ctrl1(uint8_t odr, uint8_t mode, uint8_t lp_mode)
{
    uint8_t val = 0;
    val = ((odr & 0x0F) << 4) | ((mode & 0x03) << 2) | (lp_mode & 0x03);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL1, &val, 1);
}

/* ============================================================
 *                  CTRL2 (0x21) 配置
 * ============================================================ */

/* CTRL2 位域：
 *   bit7 BOOT
 *   bit6 SOFT_RESET
 *   bit5 必须为 0
 *   bit4 CS_PU_DISC
 *   bit3 BDU
 *   bit2 IF_ADD_INC
 *   bit1 I2C_DISABLE
 *   bit0 SIM
 */
static int iis2dlpc_set_bdu(uint8_t enable)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL2, &val, 1) != 0) return -1;
    if (enable) val |= (1 << 3);
    else        val &= ~(1 << 3);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL2, &val, 1);
}

static int iis2dlpc_set_auto_increment(uint8_t enable)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL2, &val, 1) != 0) return -1;
    if (enable) val |= (1 << 2);
    else        val &= ~(1 << 2);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL2, &val, 1);
}

static int iis2dlpc_soft_reset(void)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL2, &val, 1) != 0) return -1;
    val |= (1 << 6);   /* SOFT_RESET = 1 */
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL2, &val, 1);
}

/* ============================================================
 *                  CTRL3 (0x22) 配置
 * ============================================================ */

/* CTRL3 位域：
 *   bit[7:6] ST[1:0]
 *   bit5 PP_OD
 *   bit4 LIR
 *   bit3 H_LACTIVE
 *   bit2 必须为 0
 *   bit1 SLP_MODE_SEL
 *   bit0 SLP_MODE_1
 */
static int iis2dlpc_set_pin_polarity(uint8_t active_high)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL3, &val, 1) != 0) return -1;
    if (active_high) val &= ~(1 << 3);   /* H_LACTIVE = 0 -> 高有效 */
    else             val |=  (1 << 3);   /* H_LACTIVE = 1 -> 低有效 */
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL3, &val, 1);
}

static int iis2dlpc_set_pin_mode(uint8_t push_pull)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL3, &val, 1) != 0) return -1;
    if (push_pull) val &= ~(1 << 5);     /* PP_OD = 0 -> 推挽 */
    else           val |=  (1 << 5);     /* PP_OD = 1 -> 开漏 */
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL3, &val, 1);
}

/* ============================================================
 *                  CTRL6 (0x25) 配置
 * ============================================================ */

/* CTRL6 位域：
 *   bit[7:6] BW_FILT[1:0]
 *   bit[5:4] FS[1:0]
 *   bit3 FDS
 *   bit2 LOW_NOISE
 *   bit[1:0] 必须为 0
 */
static int iis2dlpc_set_full_scale(uint8_t fs)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL6, &val, 1) != 0) return -1;
    val &= ~(0x03 << 4);
    val |=  ((fs & 0x03) << 4);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL6, &val, 1);
}

/* ============================================================
 *                  6D 方向检测配置
 * ============================================================ */

/* TAP_THS_X (0x30) 位域：
 *   bit[7]   4D_EN
 *   bit[6:5] 6D_THS[1:0]
 *   bit[4:0] TAP_THSX[4:0]
 */
static int iis2dlpc_set_6d_threshold(uint8_t ths)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_TAP_THS_X, &val, 1) != 0) return -1;
    val &= ~(0x03 << 5);
    val |=  ((ths & 0x03) << 5);
    return iis2dlpc_write_reg(IIS2DLPC_REG_TAP_THS_X, &val, 1);
}

static int iis2dlpc_set_4d_mode(uint8_t enable)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_TAP_THS_X, &val, 1) != 0) return -1;
    if (enable) val |=  (1 << 7);
    else        val &= ~(1 << 7);
    return iis2dlpc_write_reg(IIS2DLPC_REG_TAP_THS_X, &val, 1);
}

/* CTRL7 (0x3F) 位域：
 *   bit7 DRDY_PULSED
 *   bit6 INT2_ON_INT1
 *   bit5 INTERRUPTS_ENABLE
 *   bit4 USR_OFF_ON_OUT
 *   bit3 USR_OFF_ON_WU
 *   bit2 USR_OFF_W
 *   bit1 HP_REF_MODE
 *   bit0 LPASS_ON6D
 */
static int iis2dlpc_set_lpass_on6d(uint8_t lpf2)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL7, &val, 1) != 0) return -1;
    if (lpf2) val |=  (1 << 0);
    else      val &= ~(1 << 0);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL7, &val, 1);
}

static int iis2dlpc_set_interrupts_enable(uint8_t enable)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL7, &val, 1) != 0) return -1;
    if (enable) val |=  (1 << 5);
    else        val &= ~(1 << 5);
    return iis2dlpc_write_reg(IIS2DLPC_REG_CTRL7, &val, 1);
}

/* CTRL4 (0x23) 位域：
 *   bit7 INT1_6D
 *   bit6 INT1_SINGLE_TAP
 *   bit5 INT1_WU
 *   bit4 INT1_FF
 *   bit3 INT1_TAP
 *   bit2 INT1_DIFF5
 *   bit1 INT1_FTH
 *   bit0 INT1_DRDY
 */
static int iis2dlpc_route_6d_to_int1(uint8_t enable)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_CTRL4_INT1_PAD, &val, 1) != 0) return -1;
    if (enable) val |=  (1 << 7);
    else        val &= ~(1 << 7);
    if (iis2dlpc_write_reg(IIS2DLPC_REG_CTRL4_INT1_PAD, &val, 1) != 0) return -1;

    /* 同时使能中断总开关 */
    return iis2dlpc_set_interrupts_enable(1);
}

/* ============================================================
 *                  6D 中断配置
 * ============================================================ */

static void IIS2DLPC_6D_Config(void)
{
    iis2dlpc_set_6d_threshold(IIS2DLPC_6D_THS);
    iis2dlpc_set_4d_mode(1);              /* 只判断 4 个侧面 */
    iis2dlpc_set_lpass_on6d(0);           /* ODR/2 低通 */
    iis2dlpc_route_6d_to_int1(1);
}

/* ============================================================
 *                  初始化
 * ============================================================ */

void IIS2DLPC_Init(void)
{
    uint8_t whoami = 0;

    /* 1. 校验器件 ID */
    if (iis2dlpc_read_reg(IIS2DLPC_REG_WHO_AM_I, &whoami, 1) != 0) {
        return;   /* I2C 通信失败 */
    }
    if (whoami != IIS2DLPC_ID) {
        return;   /* 器件型号不对 */
    }

    /* 2. 软复位 */
    iis2dlpc_soft_reset();
    HAL_Delay(10);

    /* 3. 量程 */
    iis2dlpc_set_full_scale(IIS2DLPC_FS);

    /* 4. 工作模式 + ODR
     *    高性能模式：MODE=01, LP_MODE=00
     *    低功耗模式：MODE=00, LP_MODE=0~3
     *    这里根据 IIS2DLPC_MODE 宏自动区分
     */
    if (IIS2DLPC_MODE == IIS2DLPC_MODE_HIGH_PERF) {
        iis2dlpc_set_ctrl1(IIS2DLPC_ODR, 0x01, 0x00);
    } else {
        iis2dlpc_set_ctrl1(IIS2DLPC_ODR, 0x00, IIS2DLPC_MODE & 0x03);
    }

    /* 5. 块数据更新 */
    iis2dlpc_set_bdu(1);

    /* 6. 地址自增 */
    iis2dlpc_set_auto_increment(1);

    /* 7. 中断引脚：高有效 + 推挽 */
    iis2dlpc_set_pin_polarity(1);   /* 高有效 */
    iis2dlpc_set_pin_mode(1);       /* 推挽 */

    /* 8. 配置 6D 中断 */
    IIS2DLPC_6D_Config();
}

/* ============================================================
 *                  数据读取
 * ============================================================ */

uint8_t IIS2DLPC_WhoAmI(void)
{
    uint8_t whoami = 0;
    iis2dlpc_read_reg(IIS2DLPC_REG_WHO_AM_I, &whoami, 1);
    return whoami;
}

uint8_t IIS2DLPC_6D_GetStatus(void)
{
    uint8_t val = 0;
    if (iis2dlpc_read_reg(IIS2DLPC_REG_SIXD_SRC, &val, 1) != 0) {
        return 0;
    }
    /* SIXD_SRC bit6 = 6D_IA */
    return (val >> 6) & 0x01;
}

void IIS2DLPC_ReadRaw(int16_t *raw)
{
    uint8_t buf[6];

    if (raw == NULL) return;

    if (iis2dlpc_read_reg(IIS2DLPC_REG_OUT_X_L, buf, 6) != 0) {
        raw[0] = raw[1] = raw[2] = 0;
        return;
    }

    raw[0] = (int16_t)((buf[1] << 8) | buf[0]);
    raw[1] = (int16_t)((buf[3] << 8) | buf[2]);
    raw[2] = (int16_t)((buf[5] << 8) | buf[4]);
}

void IIS2DLPC_ReadAccel(float *x, float *y, float *z)
{
    int16_t raw[3];
    float sens;

    if (x == NULL || y == NULL || z == NULL) return;

    IIS2DLPC_ReadRaw(raw);

    switch (IIS2DLPC_FS) {
        case IIS2DLPC_FS_2G:  sens = 0.061f;  break;
        case IIS2DLPC_FS_4G:  sens = 0.122f;  break;
        case IIS2DLPC_FS_8G:  sens = 0.244f;  break;
        case IIS2DLPC_FS_16G: sens = 0.488f;  break;
        default:              sens = 0.061f;  break;
    }

    *x = (float)raw[0] * sens / 1000.0f;
    *y = (float)raw[1] * sens / 1000.0f;
    *z = (float)raw[2] * sens / 1000.0f;
}

float IIS2DLPC_GetTiltAngle(void)
{
    float x, y, z;
    float norm, cos_theta;
	float fAngle = 0.0;

    IIS2DLPC_ReadAccel(&x, &y, &z);

    norm = sqrtf(x*x + y*y + z*z);
    if (norm < 0.01f) {
        return 0.0f;
    }

    cos_theta = z / norm;
    if (cos_theta >  1.0f) cos_theta =  1.0f;
    if (cos_theta < -1.0f) cos_theta = -1.0f;

	fAngle = acosf(cos_theta) * 180.0f / 3.14159265f;
	stu_sensor.sAngle[stu_sensor.collectCnt] = (unsigned short)(fAngle * 10);

    return fAngle;
}

float IIS2DLPC_ReadTemperature(void)
{
    uint8_t buf[2];
    int16_t raw;

    if (iis2dlpc_read_reg(IIS2DLPC_REG_OUT_T_L, buf, 2) != 0) {
        return -999.0f;
    }

    raw = (int16_t)((buf[1] << 8) | buf[0]);

    /* 数据手册 Table 6：1 LSB = 1/16 ℃，25℃ 时输出 0 LSB
     * 注意：温度寄存器是 12 位，左对齐，需要右移 4 位
     */
    raw >>= 4;
    return ((float)raw / 16.0f) + 25.0f;
}


