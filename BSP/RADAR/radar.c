#include "radar.h"

unsigned char ldreadbuff[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

void Radarrcv(void)
{
	unsigned char uchData[radarLength + 1] = {0};
	unsigned char uchLength = 0;
	unsigned char uchDataN[3] = {0};

	uchLength = ring_buf_available(&rb_usart5);
	if (uchLength >= radarLength)
	{
		while (uchLength >= radarLength)
		{
			ring_buf_peek_n(&rb_usart5, uchDataN, 3);

			if(uchDataN[0] != 0x01 && uchDataN[1] != 0x03 && uchDataN[2] != 0x02)
			{
				ring_buf_drop(&rb_usart5);
			}
			else{
				ring_buf_read(&rb_usart5, uchData, radarLength);
				praseRadarData(uchData);
				ring_buf_clear(&rb_usart5);
				break;
			}

			uchLength = ring_buf_available(&rb_usart5);
		}
	}
}

void praseRadarData(unsigned char uchData[])
{
	unsigned short calc_crc = 0;  // 计算CRC
    unsigned short recv_crc = 0;  // 接收到的CRC
	unsigned short Data;

	recv_crc = uchData[radarLength - 2] | (uchData[radarLength - 1] << 8);
	calc_crc = Modbus_CRC16(uchData, radarLength);

	if (recv_crc == calc_crc)
	{
		Data = ((uint16_t)uchData[3] << 8) | (uchData[4] & 0xff);

		stu_sensor.sLD[stu_sensor.collectCnt] = Data;
	}
}

void LD8001_Read(void)
{
	uartSendIT(5, ldreadbuff, sizeof(ldreadbuff));
}
