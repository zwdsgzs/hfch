#ifndef __RADAR_H
#define __RADAR_H

#include "main.h"
#include <stdbool.h>

#define radarLength	7

typedef struct{
	bool state;	//配置模式是否激活

	bool prodid; //产品id
	bool dn; 	//域名
	bool devname; //设备id
	bool devSecret; //密钥

	unsigned char uchReply;	//1:进入配置模式，2：退出配置模式。
							//3：传输产品id。4：传输域名。5：传输设备id。6：传输密钥
}struMasterUart;

extern struMasterUart stu_Uart;

void Radarrcv(void);
void praseRadarData(unsigned char uchData[]);
void LD8001_Read(void);

void rcvAndSend(void);

void TTL_HandleFrame(unsigned char *frame, unsigned char totalLen);
void replyData(unsigned char uchData);

#endif 
