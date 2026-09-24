#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#include "main.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define RING_BUF_SIZE 256 //必须是2的n次幂

//#define uint8_t		unsigned char
//#define uint16_t	unsigned short
//#define uint32_t	unsigned int
//
//#define int8_t		char
//#define int16_t		short
//#define int32_t		int

typedef struct {
    unsigned char  buffer[RING_BUF_SIZE];
    volatile uint16_t head;   // 写入位置（生产者修改）
    volatile uint16_t tail;   // 读取位置（消费者修改）
} ring_buffer_t;

extern ring_buffer_t rb_usart1;
extern ring_buffer_t rb_usart2;
extern ring_buffer_t rb_usart4;
extern ring_buffer_t rb_usart5;


typedef struct{
	unsigned char collectCnt;	//采集第几组数据

	unsigned char uchBattery;

	unsigned short usCH4;	//甲烷
	unsigned short usH2S;	//硫化氢
	unsigned short usLD;	//雷达液位
	unsigned short usTemp;	//温度
	unsigned short usHum;	//湿度
	unsigned short usAngle;	//倾角

	unsigned short batter;	//电池电量
	
	unsigned short sH2S[5];	//硫化氢
	unsigned short sLD[5];	//雷达液位
	unsigned short sTemp[5];//温度
	unsigned short sHum[5];	//湿度
	unsigned short sAngle[5];//倾角

	unsigned int unBatEle;
}struSensorDef;
extern struSensorDef stu_sensor;


void ringBufInit(unsigned char uchName);
void ring_buf_init(ring_buffer_t *rb);
bool ring_buf_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len);
uint16_t ring_buf_read(ring_buffer_t *rb, uint8_t *out, uint16_t max_len);
uint16_t ring_buf_available(ring_buffer_t *rb);
bool ring_buf_is_empty(ring_buffer_t *rb);
bool ring_buf_peek(ring_buffer_t *rb, uint8_t *out);
uint16_t ring_buf_peek_n(ring_buffer_t *rb, uint8_t *out, uint16_t len);
bool ring_buf_drop(ring_buffer_t *rb);
void ring_buf_clear(ring_buffer_t *rb);

int BCDToDecimal(int bcd);
float str_to_float(const unsigned char *str);
unsigned int hex_string_to_int(const unsigned char *str);
double round_to_two_decimal_places(float number);

uint16_t Modbus_CRC16(uint8_t* data, size_t length);

unsigned short getusData(unsigned short usData[]);

#endif 
