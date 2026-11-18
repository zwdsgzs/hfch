#include "HLK-LD2413.h"
#include <stdlib.h>
unsigned int  HLK_LD2413_Distance=0;
uint8_t HLK_timeout =0; 
unsigned char HLK_LD2413_buf[20],HLK_LD2413_n;



#if LADAR_HLK
unsigned char LD_enablebuf[14]={0xFD, 0xFC ,0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0x03, 0x02, 0x01};
unsigned char LD_setbuf[14]={0xFD, 0xFC ,0xFB, 0xFA, 0x04, 0x00, 0x71, 0x00, 0xE8, 0x03, 0x04, 0x03, 0x02, 0x01};
unsigned char LD_setminbuf[14]={0xFD, 0xFC ,0xFB, 0xFA, 0x04, 0x00, 0x74, 0x00, 0x96, 0x00, 0x04, 0x03, 0x02, 0x01};
unsigned char LD_setmaxbuf[14]={0xFD, 0xFC ,0xFB, 0xFA, 0x04, 0x00, 0x75, 0x00, 0x10, 0x27, 0x04, 0x03, 0x02, 0x01};
unsigned char LD_disablebuf[12]={0xFD, 0xFC ,0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0x03, 0x02, 0x01};

//F4 F3 F2 F1 2 字节 4 字节（Float类型）F8 F7 F6 F5
void LADAR_Rcv_One_Char(unsigned char Data)
{
		static unsigned char buf_Head[4];
			
		buf_Head[0]=buf_Head[1];
		buf_Head[1]=buf_Head[2];
		buf_Head[2]=buf_Head[3];
		buf_Head[3]=Data;
		
		if(buf_Head[0]==0xF4&&buf_Head[1]==0xF3&&buf_Head[2]==0xF2&&buf_Head[3]==0xF1)
		{
			HLK_LD2413_buf[0]=0xF4;
			HLK_LD2413_buf[1]=0xF3;
			HLK_LD2413_buf[2]=0xF2;
			HLK_LD2413_n=3;	
			HLK_timeout=1;
		}
		if(HLK_LD2413_n < sizeof(HLK_LD2413_buf))
			HLK_LD2413_buf[HLK_LD2413_n++]=Data;	
		HLK_LD2413();
}



void HLK_LD2413(void)
{
	Radar_Data Data;
	
	if(HLK_LD2413_n>13)
	{
		if(HLK_LD2413_buf[0]==0xF4&&HLK_LD2413_buf[1]==0xF3&&HLK_LD2413_buf[2]==0xF2&&HLK_LD2413_buf[3]==0xF1)
		{
				//F4 F3 F2 F1 04 00 9C 93 B0 45 F8 F7 F6 F5 
			Data.data=(HLK_LD2413_buf[9]<<24)+(HLK_LD2413_buf[8]<<16)+(HLK_LD2413_buf[7]<<8)+(HLK_LD2413_buf[6]&0xff);
//			printf("[%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x--%02x]-%02x,%.2f\n",HLK_LD2413_buf[0],HLK_LD2413_buf[1],HLK_LD2413_buf[2],HLK_LD2413_buf[3],HLK_LD2413_buf[4],HLK_LD2413_buf[5],HLK_LD2413_buf[6],HLK_LD2413_buf[7],HLK_LD2413_buf[8],HLK_LD2413_buf[9],HLK_LD2413_buf[10],HLK_LD2413_buf[11],HLK_LD2413_buf[12],HLK_LD2413_buf[13],Data.data,Data.value.Radar_Val);
			HLK_LD2413_Distance = (unsigned int)Data.value.Radar_Val;
//			printf("HLK_LD2413_Distance = %d,HLK_timeout=[%d]\n\r",HLK_LD2413_Distance,HLK_timeout);

//			stu_sensor.usLD = HLK_LD2413_Distance;
			stu_sensor.sLD[stu_sensor.uchH2SCnt] = HLK_LD2413_Distance;
			if(HLK_LD2413_Distance !=0)
			{
				HLK_LD2413_Distance = 0;
			}
		}
	
		HLK_LD2413_n=0;
		memset(HLK_LD2413_buf,0,sizeof(HLK_LD2413_buf));
	}
	
}

unsigned int Get_HLK_LD2413(void)
{
	if(HLK_timeout!=1)
	{
		HLK_LD2413_Distance = 0xFFFF;
	}
	else
	{
		return HLK_LD2413_Distance;
	}
	
	return HLK_LD2413_Distance;
}

void SetSendTime(void)
{
	USART_UART_Trans(CM_USART6,LD_enablebuf,14,1000);
	DDL_DelayMS(500);
	USART_UART_Trans(CM_USART6,LD_setbuf,14,1000);
	DDL_DelayMS(500);
	USART_UART_Trans(CM_USART6,LD_setminbuf,14,1000);
	DDL_DelayMS(500);
	USART_UART_Trans(CM_USART6,LD_setmaxbuf,14,1000);
	DDL_DelayMS(500);
	USART_UART_Trans(CM_USART6,LD_disablebuf,12,1000);
	DDL_DelayMS(500);
}

#else
unsigned char ldreadbuff[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};
int rcvlen = 0;
unsigned char peekdata = 0;

void LADAR_Rcv_One_Char(unsigned char Data)
{
		static unsigned char buf_Head[4];

		buf_Head[0]=buf_Head[1];
		buf_Head[1]=buf_Head[2];
		buf_Head[2]=Data;

		if(buf_Head[0]==0x01&&buf_Head[1]==0x03&&buf_Head[2]==0x02)
		{
			HLK_LD2413_buf[0]=0x01;
			HLK_LD2413_buf[1]=0x03;
			HLK_LD2413_n=2;
			HLK_timeout=1;
		}
		if(HLK_LD2413_n < sizeof(HLK_LD2413_buf))
			HLK_LD2413_buf[HLK_LD2413_n++]=Data;
		LD8001H();
}

void LD8001H(void)
{
	uint16_t Data;
	
	if(HLK_LD2413_n>=7)
	{
		if(HLK_LD2413_buf[0]==0x01&&HLK_LD2413_buf[1]==0x03&&HLK_LD2413_buf[2]==0x02)
		{
			Data=((uint16_t)HLK_LD2413_buf[3]<<8)|(HLK_LD2413_buf[4]&0xff);

//			stu_sensor.usLD = Data;
			stu_sensor.sLD[stu_sensor.uchH2SCnt] = Data;
		}

		HLK_LD2413_n=0;
		memset(HLK_LD2413_buf,0,sizeof(HLK_LD2413_buf));
	}
}

void LD8001_Read(void)
{
	USART_UART_Trans(CM_USART6, ldreadbuff, sizeof(ldreadbuff), 1000);
}

#endif

