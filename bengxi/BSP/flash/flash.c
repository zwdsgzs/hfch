#include "flash.h"

_Para Para;
_Para ParaTemp;//与json通信

#define FLASHPARA_INIT 0	// 0：flash不初始化数据。1：flash初始化数据

/*
	对mqtt参数进行初始化默认值
	mqtt参数:三元组"ProductKey"，  "DeviceName"，  "DeviceSecret"，
			连接参数：mqttBroker，mqttClientID，port：1833
*/
void ParaInit(void)
{ 
	strcpy(Para.deviceName,"dev_0001");//经测试，纯数字id最多8位，带英文id最多9位
	strcpy(Para.deviceSecret,"dWg0QXhnQW5TVWEzVFNPelBwQkFTQWVGNHZmaVF3aXk=");

	strcpy(Para.mqttBroker,"z8k09jGpX5.mqtts.acc.cmcconenet.cn");
	strcpy(Para.productKey,"z8k09jGpX5");

	stu_sensor.uchCount = 2;
	Para.updateIntervalmins=24*60/stu_sensor.uchCount;//计算出休眠时间
}

// 显示mqtt参数
void ParaShow(void)
{
	printf("\r\ndeviceName=%s\n",Para.deviceName);
	printf("deviceSecret=%s\n",Para.deviceSecret);
	printf("mqttBroker=%s\n",Para.mqttBroker);
	printf("productKey=%s\n",Para.productKey);
}

/****
	关闭中断才能写入flash
****/

void ParaWrite(void)
{	
	uint32_t u32Addr;
//	uint32_t i;

	Para.FirstFlag=0x99;
//	uint8_t  *s = (uint8_t *)&Para;
						
	Interrupt_Off();
	SysTick_Suspend();	//中断关闭
					 
	
	LL_PERIPH_WE(LL_PERIPH_ALL);
	
	/* Wait flash ready. */
	while (SET != EFM_GetStatus(EFM_FLAG_RDY)) {;}	
  //printf("------1-----\n");
	
	/* EFM_FWMC write enable */
	EFM_FWMC_Cmd(ENABLE);	
	
	/* Release bus while erase & program */
	EFM_SetBusStatus(EFM_BUS_HOLD);		
  //printf("------2-----\n");
	
	(void)EFM_SingleSectorOperateCmd(EFM_SECTOR_NUM, ENABLE);
	u32Addr = EFM_SECTOR_ADDR(EFM_SECTOR_NUM);	
	//printf("------3-----\n");
	
	/* Sector erase */
	(void)EFM_SectorErase(u32Addr);
	//printf("------4-----\n");
	
	/* Sequence program */	
	(void)EFM_SequenceProgram(u32Addr, (uint8_t *)&Para, sizeof(Para));
	//printf("------5-----\n");
	
	/* Read back */	
	//(void)EFM_ReadByte(u32Addr, u8ReadBuf, sizeof(Para));
	
	/* Write protection */
	(void)EFM_SingleSectorOperateCmd(EFM_SECTOR_NUM, DISABLE);	
  //printf("------6-----\n");
	
	EFM_FWMC_Cmd(DISABLE);
	
	LL_PERIPH_WP(LL_PERIPH_ALL); 
	
	SysTick_Resume();
	Interrupt_ON();	
	
	ParaShow();
}

void ParaRead(void)//读取flash
{
  unsigned char LBuff[350];

	while (SET != EFM_GetStatus(EFM_FLAG_RDY)) {;}

	uint32_t u32Addr0 = EFM_SECTOR_ADDR(EFM_SECTOR_NUM);
	(void)EFM_ReadByte(u32Addr0, LBuff, sizeof(Para));	

  PPara pPara = (PPara)&LBuff;
  memcpy(&Para, pPara, sizeof(Para));
}

void Flash_Init(void)
{
	  ParaRead();
	  if(Para.FirstFlag!=0x99)
		{
			ParaInit();
			ParaWrite();
		}
#if FLASHPARA_INIT == 1
	ParaInit();
#endif
}



void flash_copy(void)
{	
	strcpy((char *)ParaTemp.deviceName,(char *)Para.deviceName);
	strcpy((char *)ParaTemp.productKey,(char *)Para.productKey);
	strcpy((char *)ParaTemp.deviceSecret,(char *)Para.deviceSecret);
	strcpy((char *)ParaTemp.mqttBroker,(char *)Para.mqttBroker);
	ParaTemp.updateIntervalmins = Para.updateIntervalmins;
}

