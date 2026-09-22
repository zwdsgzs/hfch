#ifndef __EEPROM_H
#define __EEPROM_H

#include "main.h"  //包含需要的头文件

/* ============================================================
 *                      EEPROM 地址规划
 * ============================================================ */

#define PARA_MAGIC_VALUE    0xA1A2A3A4
#define PARA_MAGIC_ADDR     0x08080000                  /* 魔数地址 */
#define PARA_DATA_ADDR      (PARA_MAGIC_ADDR + 4)       /* 参数区地址 */
#define PARA_DATA_SIZE      sizeof(flshDataDef)         /* 200 字节 */


//STM32L0系列有内部eeprom用来存储配置数据，大小6K
#define DATA_EEPROM_START_ADDR		0x08080000   //起始地址
#define DATA_EEPROM_END_ADDR     	0x080817FF  // 6KB = 0x1800

#define USER_DATA_EEPROM_ADDR		DATA_EEPROM_START_ADDR//用户地址
#define USER_EEPROM_BYTE_SIZE		0x01FF				//空间	512字节
#define iEEPROM_CHECK_NUM 			2


typedef struct
{
	char   productKey[32];// Json---Mid
	char   deviceName[32];// Json---Mid
	char   deviceSecret[52];// Json---Mid

	char   mqttBroker[52];// Json---Mid

	unsigned int   updateIntervalmins;

	unsigned int   unLat;//纬度*10000
	unsigned int   unLon;//经度*10000

	double   dbLat;
	double   dbLon;
}flshDataDef;
extern flshDataDef stu_flash;	//当前生效的参数
extern flshDataDef stu_flashjson;	//从 JSON 解析出来的新参数，用于和 stu_flash 对比


HAL_StatusTypeDef EEPROM_WriteData(uint32_t Address, uint32_t *wData, uint32_t len); 
HAL_StatusTypeDef EEPROM_ReadData(uint32_t Address, uint32_t *rData, uint32_t len);

HAL_StatusTypeDef EEPROM_WRITE_Verify_CHECK(uint32_t Address, uint32_t *wData, uint32_t len);


/* ============================================================
 *                      API
 * ============================================================ */

//从 EEPROM 读取参数到 stu_flash
int  flashReadFromEEPROM(void);

//将 stu_flash 写入 EEPROM
int  flashWriteToEEPROM(void);

//初始化flash数据
void flashDataInit(void);

#endif
