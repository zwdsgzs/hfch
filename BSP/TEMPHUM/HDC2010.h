#ifndef  __HDC2010_H__
#define  __HDC2010_H__

#include "main.h"

#define HDC2010_ADD	0x40

// 寄存器地址
#define HDC2010_TEMP_LOW        	0x00
#define HDC2010_TEMP_HIGH       	0x01
#define HDC2010_HUMID_LOW       	0x02
#define HDC2010_HUMID_HIGH      	0x03
#define HDC2010_INTERRUPT_DRDY  	0x04
#define HDC2010_TEMP_MAX            0x05
#define HDC2010_HUMID_MAX           0x06
#define HDC2010_INTERRUPT_CONFIG    0x07
#define HDC2010_TEMP_OFFSET         0x08
#define HDC2010_HUM_OFFSET          0x09
#define HDC2010_TEMP_THR_L          0x0A
#define HDC2010_TEMP_THR_H          0x0B
#define HDC2010_HUMID_THR_L         0x0C
#define HDC2010_HUMID_THR_H         0x0D
#define HDC2010_CONFIG              0x0E
#define HDC2010_MEASUREMENT_CONFIG  0x0F
#define HDC2010_DEVICE_ID_L         0xFE
#define HDC2010_DEVICE_ID_H         0xFF

/* ---------- 分辨率 ---------- */
#define HDC2010_RES_14BIT           0
#define HDC2010_RES_11BIT           1
#define HDC2010_RES_9BIT            2

/* ---------- 测量模式 ---------- */
#define HDC2010_MEAS_TEMP_HUMID     0
#define HDC2010_MEAS_TEMP_ONLY      1
#define HDC2010_MEAS_HUMID_ONLY     2

/* ---------- 采样率 ---------- */
#define HDC2010_RATE_MANUAL         0
#define HDC2010_RATE_2MIN           1
#define HDC2010_RATE_1MIN           2
#define HDC2010_RATE_10S            3
#define HDC2010_RATE_5S             4
#define HDC2010_RATE_1HZ            5
#define HDC2010_RATE_2HZ            6
#define HDC2010_RATE_5HZ            7

/* ---------- 中断模式 ---------- */
#define HDC2010_INT_LEVEL           0
#define HDC2010_INT_COMPARATOR      1

/* ---------- 中断极性 ---------- */
#define HDC2010_INT_ACTIVE_LOW      0
#define HDC2010_INT_ACTIVE_HIGH     1

void HDC2010_Init(void);

void HDC2010TriggerMeasurement(void);//开始转换
float HDC2010ReadTemp(void);//读取温度
float HDC2010ReadHumidity(void);//读取湿度

#endif
