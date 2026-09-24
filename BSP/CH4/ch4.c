#include "ch4.h"
extern int leng;
void laserCH4rcv(void)
{
	unsigned char uchData[ch4Length + 1] = {0};
	unsigned char uchLength = 0;
	unsigned char uchData0 = 0;

	
	if((rb_usart4.head - rb_usart4.tail) != 0)
	{
		uchLength = 1;
	}
	uchLength = ring_buf_available(&rb_usart4);
	if (uchLength >= ch4Length)
	{
		while (uchLength >= ch4Length)
		{
			ring_buf_peek(&rb_usart4, &uchData0);

			if (uchData0 != '+')
			{
				ring_buf_drop(&rb_usart4);
			}
			else{
				ring_buf_read(&rb_usart4, uchData, ch4Length);
				praseCH4Data(uchData);
				ring_buf_clear(&rb_usart4);
				break;
			}

			uchLength = ring_buf_available(&rb_usart4);
		}
		
	}
}

void praseCH4Data(unsigned char uchData[])
{
	unsigned char check = 0;
	unsigned int temp = 0;
	unsigned char buf[ch4Length + 1] = {0};
	int i = 0;
	float CH4_percent = 0.0;
	float lel = 0.0;

	// check
	for(i = 0; i < 17; i++)
	{
		check = check ^ uchData[i];
	}
	buf[0] = uchData[17];
	buf[1] = uchData[18];
    buf[2] = '\0';

	temp = hex_string_to_int(buf);
	if(temp!=check)
	{
		return;
	}

	memset(buf,0,sizeof(buf));	
	for(i = 0; i < 7; i++)
	{
	  buf[i] = uchData[i];
	}	
	buf[7] = '\0';
	CH4_percent = str_to_float(buf);
	lel = CH4_percent/5.0 * 100.0;
	stu_sensor.usCH4 = (unsigned short)lel;
}

