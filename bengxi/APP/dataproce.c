#include "dataproce.h"

sensorDataDef stu_sensor = {0};

// 计算CRC16（Modbus RTU格式）
uint16_t Modbus_CRC16(uint8_t* data, size_t length)
{
    uint16_t crc = 0xFFFF;  // 初始值
    uint8_t i;

    for (size_t idx = 0; idx < length; idx++)
    {
        crc ^= data[idx];  // 异或到CRC低字节

        for (i = 0; i < 8; i++)  // 处理每个字节的8位
        {
            if (crc & 0x0001)  // 如果最低位为1
            {
                crc = (crc >> 1) ^ 0xA001;  // 右移一位并异或多项式
            }
            else
            {
                crc = crc >> 1;  // 直接右移
            }
        }
    }

    return crc;  // 返回16位CRC值（低字节在前，高字节在后）
}

unsigned short getusData(unsigned short usData[])
{
	int i = 0;
	int max = 0;
	int min = 0;
	int num = 0;
	int sum = 0;
	unsigned short avg = 0;
	//去掉最大值\最小值
	for (i = 0; i < 4; ++i)
	{
		if (usData[max] < usData[i+1])
		{
			max = i+ 1;
		}

		if (usData[min] > usData[i+1])
		{
			min = i+ 1;
		}
	}

	//计算不为0个数
	for (i = 0; i < 5; ++i)
	{
		if ((i != min) && (i != max))
		{
			if (usData[i] != 0)
			{
				sum += usData[i];
				num ++;
			}
		}
	}

	//取平均值
	if (num > 0)
	{
		avg = (unsigned short)(sum / num);
	}

	return avg;
}


