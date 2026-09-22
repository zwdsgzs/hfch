#ifndef __IIS2DLP_H
#define __IIS2DLP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define IIS2DLP_ADD	0x18

/* ---------- 6D 阈值 ---------- */
/* 0 = 80°, 1 = 70°, 2 = 60°, 3 = 50°
 * 数值越大，阈值角度越小，越容易触发
 */
#define IIS2DLPC_6D_THS         2       /* 60° */

/* 量程 */
#define IIS2DLPC_FS_2G              0
#define IIS2DLPC_FS_4G              1
#define IIS2DLPC_FS_8G              2
#define IIS2DLPC_FS_16G             3
#define IIS2DLPC_FS                 IIS2DLPC_FS_2G

/* 工作模式 */
#define IIS2DLPC_MODE_HIGH_PERF     0x04
#define IIS2DLPC_MODE_LP4           0x03
#define IIS2DLPC_MODE_LP3           0x02
#define IIS2DLPC_MODE_LP2           0x01
#define IIS2DLPC_MODE_LP1           0x00
#define IIS2DLPC_MODE               IIS2DLPC_MODE_HIGH_PERF

/* ODR */
#define IIS2DLPC_ODR_OFF            0x00
#define IIS2DLPC_ODR_12HZ5          0x02
#define IIS2DLPC_ODR_25HZ           0x03
#define IIS2DLPC_ODR_50HZ           0x04
#define IIS2DLPC_ODR_100HZ          0x05
#define IIS2DLPC_ODR_200HZ          0x06
#define IIS2DLPC_ODR_400HZ          0x07
#define IIS2DLPC_ODR_800HZ          0x08
#define IIS2DLPC_ODR_1600HZ         0x09
#define IIS2DLPC_ODR                IIS2DLPC_ODR_100HZ


/* ============================================================
 *              寄存器地址（数据手册 Section 7）
 * ============================================================ */

#define IIS2DLPC_REG_OUT_T_L            0x0D
#define IIS2DLPC_REG_OUT_T_H            0x0E
#define IIS2DLPC_REG_WHO_AM_I           0x0F
#define IIS2DLPC_REG_CTRL1              0x20
#define IIS2DLPC_REG_CTRL2              0x21
#define IIS2DLPC_REG_CTRL3              0x22
#define IIS2DLPC_REG_CTRL4_INT1_PAD     0x23
#define IIS2DLPC_REG_CTRL5_INT2_PAD     0x24
#define IIS2DLPC_REG_CTRL6              0x25
#define IIS2DLPC_REG_OUT_T              0x26
#define IIS2DLPC_REG_STATUS             0x27
#define IIS2DLPC_REG_OUT_X_L            0x28
#define IIS2DLPC_REG_OUT_X_H            0x29
#define IIS2DLPC_REG_OUT_Y_L            0x2A
#define IIS2DLPC_REG_OUT_Y_H            0x2B
#define IIS2DLPC_REG_OUT_Z_L            0x2C
#define IIS2DLPC_REG_OUT_Z_H            0x2D
#define IIS2DLPC_REG_TAP_THS_X          0x30
#define IIS2DLPC_REG_SIXD_SRC           0x3A
#define IIS2DLPC_REG_CTRL7              0x3F

/* IIS2DLPC 器件 ID */
#define IIS2DLPC_ID                     0x44


/* ---------- 对外接口 ---------- */
void  IIS2DLPC_Init(void);

/* 读取三轴加速度，单位 g */
void  IIS2DLPC_ReadAccel(float *x, float *y, float *z);

/* 计算相对水平面的倾角（度） */
float IIS2DLPC_GetTiltAngle(void);



#ifdef __cplusplus
}
#endif

#endif /*IIS2DLPC_REGS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
