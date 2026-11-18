#ifndef __FLASH_H
#define __FLASH_H

#include "config.h"

#define EFM_SECTOR_NUM         (30U)

typedef struct
{
//	unsigned char   mqttClientID[30];// Json---Mid
	char   productKey[30];// Json---Mid
	char   deviceName[30];// Json---Mid
	char   deviceSecret[50];// Json---Mid

	char   mqttBroker[50];// Json---Mid

	unsigned int   updateIntervalmins;

	unsigned int   unLat;//纬度*10000
	unsigned int   unLon;//经度*10000

	double   dbLat;
	double   dbLon;

	unsigned char   FirstFlag;
} _Para,*PPara;
extern _Para Para;
extern _Para ParaTemp;

extern unsigned char   tempKey[30];
void ParaInit(void);
void ParaShow(void);
void ParaWrite(void);
void ParaRead(void);
void Flash_Init(void);
void flash_copy(void);


#endif 

