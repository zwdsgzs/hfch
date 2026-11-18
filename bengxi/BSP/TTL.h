#ifndef __TTL_H
#define __TTL_H

#include "config.h"

typedef struct{
	bool prodid; //产品id
	bool dn; 	//域名
	bool devname; //设备id
	bool devSecret; //密钥

	unsigned char uchReply;
}struMasterUart;

extern struMasterUart stu_Uart; 


void TTL_Rcv_One_Char(unsigned char Data);
void TTL_Handle(void);
void replyData(unsigned char uchData);

#endif 
