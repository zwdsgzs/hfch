/*lic*/
#ifndef __DATAPROCE_H__
#define __DATAPROCE_H__

#include "config.h"

#include <stdint.h>
#include <string.h>

typedef struct{
//	unsigned char uchCollectNum;	//采集计数
	bool open;	//井盖状态

	unsigned char uchLDCnt;	//雷达错误次数，记三次

	unsigned char uchCount;	//上报次数

	unsigned short usCH4;	//甲烷
	unsigned short usH2S;	//硫化氢
	unsigned short usLD;		//雷达液位

	unsigned short batter;	//电池电量
	
	
	unsigned short sH2S[5];	//硫化氢
	unsigned short sLD[5];		//雷达液位
	unsigned char uchH2SCnt; //硫化氢数量

	double lon;	//经度,模块上报
	double lat;	//纬度,模块上报

//	unsigned char usWarning;	//警告信息
}sensorDataDef;
extern sensorDataDef stu_sensor;

uint16_t Modbus_CRC16(uint8_t* data, size_t length);

unsigned short getusData(unsigned short usData[]);

#endif 

/*eof*/
