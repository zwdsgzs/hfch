#include "radar.h"

unsigned char ldreadbuff[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

unsigned char replyBuf[]={0xF1, 0xF2, 0xF3, 0xF4, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4};

struMasterUart stu_Uart={
	.state = false,
	.prodid = false,
	.dn = false,
	.devname = false,
	.devSecret = false,
	.uchReply = 0,
}; 

void Radarrcv(void)
{
	unsigned char uchData[60] = {0};//ttl字节最多55
	unsigned short usLength = 0;
	unsigned char uchDataN[7] = {0};

	usLength = ring_buf_available(&rb_usart5);
	if (usLength >= radarLength)
	{
		while (usLength >= radarLength)
		{
			ring_buf_peek_n(&rb_usart5, uchDataN, 1);
			if (uchDataN[0] == 0xF1)
			{
				/* ---------- TTL帧 ---------- */
				ring_buf_peek_n(&rb_usart5, uchDataN, 7);

				if (uchDataN[0] == 0xF1 && uchDataN[1] == 0xF2 &&
					uchDataN[2] == 0xF3 && uchDataN[3] == 0xF4)
				{
					//长度校验
					if (uchDataN[6] > (sizeof(uchData) - 12))
					{
						ring_buf_clear(&rb_usart5);
						break;
					}
					if ((uchDataN[6] + 11) <= usLength)
					{
						ring_buf_read(&rb_usart5, uchData, (uchDataN[6] + 11));
						TTL_HandleFrame(uchData, (uchDataN[6] + 11));
						ring_buf_clear(&rb_usart5);
						break;
					}
					else{
						break;//长度不够，退出
					}
				}
				else{
					ring_buf_drop(&rb_usart5);//丢一个字节
				}
			}
			else if (uchDataN[0] == 0x01)
			{
				/* ---------- 雷达帧 ---------- */
				ring_buf_peek_n(&rb_usart5, uchDataN, 3);

				if(uchDataN[0] == 0x01 && uchDataN[1] == 0x03 && uchDataN[2] == 0x02)
				{
					ring_buf_read(&rb_usart5, uchData, radarLength);
					praseRadarData(uchData);
					ring_buf_clear(&rb_usart5);
					break;
				}
				else{
					ring_buf_drop(&rb_usart5);//丢一个字节
				}
			}
			else{
				ring_buf_drop(&rb_usart5);//丢一个字节
			}

			usLength = ring_buf_available(&rb_usart5);
		}
	}
}

void praseRadarData(unsigned char uchData[])
{
	unsigned short calc_crc = 0;  // 计算CRC
    unsigned short recv_crc = 0;  // 接收到的CRC
	unsigned short Data;

	recv_crc = uchData[radarLength - 2] | (uchData[radarLength - 1] << 8);
	calc_crc = Modbus_CRC16(uchData, radarLength - 2);

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

void rcvAndSend(void)
{
	HAL_StatusTypeDef st;
	static unsigned char uchData[512] = {0};//dma在缓存区满溢后，会触发两次
	unsigned short usLength = 0;

	usLength = ring_buf_available(&rb_usart5);
	if (usLength > 0)
	{
		ring_buf_read(&rb_usart5, uchData, usLength);

		st = uartSendIT(1, uchData, usLength);
		while (st == HAL_BUSY) {
			osDelay(1);
			st = uartSendIT(1, uchData, usLength);
		}

		/* 等真正发送完成，防止下次调用覆盖 uchData */
		while (huart1.gState != HAL_UART_STATE_READY)
		{
			osDelay(1);
		}

//		st = uartSendIT(5, uchData, usLength);
//		while (st == HAL_BUSY) {
//			osDelay(1);
//			st = uartSendIT(5, uchData, usLength);
//		}
//
//		/* 等真正发送完成，防止下次调用覆盖 uchData */
//		while (huart5.gState != HAL_UART_STATE_READY)
//		{
//			osDelay(1);
//		}
	}
}

void TTL_HandleFrame(unsigned char *frame, unsigned char totalLen)
{
    unsigned short usData;
    unsigned char datalen = totalLen;

    if ((frame[datalen-1] == 0xA4) && (frame[datalen-2] == 0xA3) &&
        (frame[datalen-3] == 0xA2) && (frame[datalen-4] == 0xA1))
    {
        usData = (unsigned short)frame[4] << 8 | frame[5];

        switch (usData)
        {
        case 1:
            stu_Uart.state = true;
            stu_Uart.uchReply = 1;
            break;
		case 2:
			stu_Uart.state = false;
            stu_Uart.uchReply = 2;
            break;
        case 3:
            memset(stu_flash.productKey, 0, sizeof(stu_flash.productKey));
            memcpy(stu_flash.productKey, &frame[7], frame[6]);
            stu_Uart.prodid = true;
            stu_Uart.uchReply = 3;
            break;
        case 4:
            memset(stu_flash.mqttBroker, 0, sizeof(stu_flash.mqttBroker));
            memcpy(stu_flash.mqttBroker, &frame[7], frame[6]);
            stu_Uart.dn = true;
            stu_Uart.uchReply = 4;
            break;
        case 5:
            memset(stu_flash.deviceName, 0, sizeof(stu_flash.deviceName));
            memcpy(stu_flash.deviceName, &frame[7], frame[6]);
            stu_Uart.devname = true;
            stu_Uart.uchReply = 5;
            break;
        case 6:
            memset(stu_flash.deviceSecret, 0, sizeof(stu_flash.deviceSecret));
            memcpy(stu_flash.deviceSecret, &frame[7], frame[6]);
            stu_Uart.devSecret = true;
            stu_Uart.uchReply = 6;
            break;
        }
        replyData(stu_Uart.uchReply);
    }
}

void replyData(unsigned char uchData)
{
	replyBuf[4] = uchData;

	uartSendIT(5, replyBuf, sizeof(replyBuf)); 
}

