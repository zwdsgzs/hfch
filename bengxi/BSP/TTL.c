#include "TTL.h"

extern StateMachine sm;

struMasterUart stu_Uart={
	.prodid = false,
	.dn = false,
	.devname = false,
	.devSecret = false,
}; 

unsigned char replyBuf[10]={0xF1, 0xF2, 0xF3, 0xF4, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4};
unsigned char TTL_buf[150],TTL_Buf_n,datalen;

void TTL_Rcv_One_Char(unsigned char Data)
{
	static unsigned char buf_Head[4];
	RS485_timeout=1;
	buf_Head[0]=buf_Head[1];
	buf_Head[1]=buf_Head[2];
	buf_Head[2]=buf_Head[3];
	buf_Head[3]=Data;

	if((buf_Head[0]==0xF1)&&(buf_Head[1]==0xF2)&&(buf_Head[2]==0xF3)&&(buf_Head[3]==0xF4))
	{
		TTL_buf[0]=0xF1;
		TTL_buf[1]=0xF2;
		TTL_buf[2]=0xF3;
		TTL_Buf_n=3;
	}
	if(TTL_Buf_n < sizeof(TTL_buf))
		TTL_buf[TTL_Buf_n++]=Data;

	if (TTL_Buf_n >= 7)
	{
		datalen = TTL_buf[6] + 11;
	}
	else{
		datalen = 7;
	}
	TTL_Handle();
}


void TTL_Handle(void)
{
	unsigned short usData = 0;
	if(TTL_Buf_n >= datalen)
	{
		if((TTL_buf[datalen-1]==0xA4)&&(TTL_buf[datalen-2]==0xA3)
		   &&(TTL_buf[datalen-3]==0xA2)&&(TTL_buf[datalen-4]==0xA1))
		{
			usData = (unsigned short)TTL_buf[4] << 8 | TTL_buf[5];

			switch (usData)
			{
			case 1:
				sm.state = STATE_CONFIG;
				sm.counter = 0;
				stu_Uart.uchReply = 1;
				GPS_POWER_SET;
				RADA_POWER_SET;
				CH4_POWER_SET;
				H2S_POWER_SET;
				MOTOR_POWER_RESET;
				break;
			case 2:
				stu_Uart.uchReply = 2;
				break;
			case 3:
				memset(Para.productKey,0,sizeof(Para.productKey));
				memcpy(Para.productKey,&TTL_buf[7],TTL_buf[6]);
				stu_Uart.prodid = true;
				stu_Uart.uchReply = 3;
				break;
			case 4:
				memset(Para.mqttBroker,0,sizeof(Para.mqttBroker));
				memcpy(Para.mqttBroker,&TTL_buf[7],TTL_buf[6]);
				stu_Uart.dn = true; 
				stu_Uart.uchReply = 4;
				break;
			case 5:
				memset(Para.deviceName,0,sizeof(Para.deviceName));
				memcpy(Para.deviceName,&TTL_buf[7],TTL_buf[6]);
				stu_Uart.devname = true; 
				stu_Uart.uchReply = 5;
				break;
			case 6:
				memset(Para.deviceSecret,0,sizeof(Para.deviceSecret));
				memcpy(Para.deviceSecret,&TTL_buf[7],TTL_buf[6]);
				stu_Uart.devSecret = true; 
				stu_Uart.uchReply = 6;
				break;
			}
		}
		
		TTL_Buf_n = 0;
		memset(TTL_buf,0,sizeof(TTL_buf));
	}
}

void replyData(unsigned char uchData)
{
	replyBuf[4] = uchData;

	USART_UART_Trans(CM_USART1, replyBuf, 10, 1000);
}


