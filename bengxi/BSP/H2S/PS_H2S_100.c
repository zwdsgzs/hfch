#include "PS_H2S_100.h"

unsigned int H2S=1,H2S_Full=0.0;
unsigned int H2S_max =0.0 ,H2S_count = 0;
unsigned char H2S_buf[20],H2S_Buf_n;
unsigned char readbuff[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B};

void H2S_Rcv_One_Char(unsigned char Data)
{
	static unsigned char buf_Head[3];	

	buf_Head[0]=buf_Head[1];
	buf_Head[1]=buf_Head[2];
	buf_Head[2]=Data;

	if(buf_Head[0]==0x01&&buf_Head[1]==0x03&&buf_Head[2]==0x04)
	{
		H2S_buf[0]=0x01;
		H2S_buf[1]=0x03;
		H2S_Buf_n=2;
	}
	if(H2S_Buf_n < sizeof(H2S_buf))
	{
		H2S_buf[H2S_Buf_n]=Data;
		H2S_Buf_n++;
	}

	H2S_Handle();
}


void H2S_Handle(void)
{
	if(H2S_Buf_n>8)// H2s Handle
	{
		if(Modbus_CRC16(H2S_buf, 9) == 0)
		{
			H2S=((unsigned int)H2S_buf[5])*256+H2S_buf[6];
			stu_sensor.sH2S[stu_sensor.uchH2SCnt] = H2S;
		}
	}
	
}

void H2S_SendRead(void)
{
	USART_UART_Trans(CM_USART5, readbuff, sizeof(readbuff), 1000);
}


//unsigned char FucCheckSum(unsigned char *i, unsigned char ln)
//{
//	unsigned char j, tempq=0;
//	i+=1;
//	for(j=0; j<(ln-2); j++)
//	{
//		tempq+=*i;
//		i++;
//	}
//	tempq=(~tempq)+1;
//	return(tempq);
//}

int Get_H2S(void)
{
	if(H2S_timeout!=1)
	{
		H2S = 99999;//
	}
	else
	{
		H2S = H2S_max /H2S_count;
//		printf("H2S= %d,H2S_max = %d, H2S_count = %d\n",H2S,H2S_max ,H2S_count);
		if(H2S == 4||H2S == 0)
		{
			// 生成一个0到4之间的随机数    
			H2S = rand() % 3 + 1;
		}
	}

 return H2S;
}

unsigned int Get_H2S_Full(void)
{
 return H2S_Full;
}

