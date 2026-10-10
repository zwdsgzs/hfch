#include "h2s.h"

unsigned char H2Sreadbuff[] = {0x43};//'C'

void H2Srcv(void)
{
	unsigned char uchData[20] = {0};
	unsigned char uchLength = 0;
	unsigned char uchData0 = 0;
	
	if((rb_usart2.head - rb_usart2.tail) != 0)
	{
		uchLength = 1;
	}
	uchLength = ring_buf_available(&rb_usart2);
	if (uchLength >= h2sLengthMax)
	{
		while (uchLength >= h2sLengthMax)
		{
			ring_buf_peek(&rb_usart2, &uchData0);

			if (uchData0 != ':')
			{
				ring_buf_drop(&rb_usart2);
			}
			else{
				ring_buf_read(&rb_usart2, uchData, h2sLengthMax);
				praseH2SData(uchData);
				ring_buf_clear(&rb_usart2);
				break;
			}

			uchLength = ring_buf_available(&rb_usart2);
		}
		
	}
}

void praseH2SData(unsigned char uchData[])
{
	unsigned char buf[10] = {0};
	int i = 0;
	float H2S_percent = 0.0;
	
	for(i = 0; i < 10; i++)
	{
	  buf[i] = uchData[i];
	}	

	H2S_percent = str_to_float(buf);
	stu_sensor.sH2S[stu_sensor.collectCnt] = (unsigned short)(H2S_percent + 0.5);
}


void H2S_Read(void)
{
	uartSendIT(2, H2Sreadbuff, sizeof(H2Sreadbuff));
}



