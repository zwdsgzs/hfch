#include "dataProcess.h"

struSensorDef stu_sensor = {0};

ring_buffer_t rb_usart1;
ring_buffer_t rb_usart2;
ring_buffer_t rb_usart4;
ring_buffer_t rb_usart5;

void ringBufInit(unsigned char uchName)
{
	switch (uchName)
	{
	case 0:
		ring_buf_init(&rb_usart1);
		ring_buf_init(&rb_usart2);
		ring_buf_init(&rb_usart4);
		ring_buf_init(&rb_usart5);
		break;
	case 1:
		ring_buf_init(&rb_usart1);
		break;
	case 2:
		ring_buf_init(&rb_usart2);
		break;
	case 4:
		ring_buf_init(&rb_usart4);
		break;
	case 5:
		ring_buf_init(&rb_usart5);
		break;
	default:break;
	}
}

void ring_buf_init(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

// 返回 true 表示全部写入成功；false 表示空间不够，只写入了部分
bool ring_buf_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len)
{
    uint16_t space = RING_BUF_SIZE - 1 - ring_buf_available(rb);
    if (len > space) {
        len = space;   // 丢弃多余数据，或者返回 false 让上层处理
    }
    for (uint16_t i = 0; i < len; i++) {
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) & (RING_BUF_SIZE - 1);
    }
    return (len == space) ? false : true;
}

// 返回实际读出的字节数
uint16_t ring_buf_read(ring_buffer_t *rb, uint8_t *out, uint16_t max_len)
{
    uint16_t avail = ring_buf_available(rb);
    uint16_t len = (avail < max_len) ? avail : max_len;
    for (uint16_t i = 0; i < len; i++) {
        out[i] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) & (RING_BUF_SIZE - 1);
    }
    return len;
}

//查看有多少数据
uint16_t ring_buf_available(ring_buffer_t *rb)
{
    return (rb->head - rb->tail) & (RING_BUF_SIZE - 1);
}

//判断是否为空
bool ring_buf_is_empty(ring_buffer_t *rb)
{
    return (rb->head == rb->tail);
}

// 窥视头部第一个字节，但不移除
// 返回值：true = 成功，*out 中是要看的字节；false = 缓冲区为空
bool ring_buf_peek(ring_buffer_t *rb, uint8_t *out)
{
    if (ring_buf_is_empty(rb)) {
        return false;
    }
    *out = rb->buffer[rb->tail];
    return true;
}

/**
 * @brief  窥视环形缓冲区头部的前 N 个字节，但不移除
 * @param  rb       环形缓冲区指针
 * @param  out      输出缓冲区，长度至少为 len
 * @param  len      想查看的字节数
 * @retval 实际拷贝的字节数（可能小于 len，如果缓冲区数据不够）
 */
uint16_t ring_buf_peek_n(ring_buffer_t *rb, uint8_t *out, uint16_t len)
{
    uint16_t avail = ring_buf_available(rb);
    uint16_t n = (avail < len) ? avail : len;
    uint16_t idx = rb->tail;

    for (uint16_t i = 0; i < n; i++) {
        out[i] = rb->buffer[idx];
        idx = (idx + 1) & (RING_BUF_SIZE - 1);
    }

    return n;
}

// 丢弃头部一个字节
// 返回值：true = 成功丢弃；false = 缓冲区为空
bool ring_buf_drop(ring_buffer_t *rb)
{
    if (ring_buf_is_empty(rb)) {
        return false;
    }
    rb->tail = (rb->tail + 1) & (RING_BUF_SIZE - 1);
    return true;
}

/**
 * @brief  清空环形缓冲区（丢弃所有未读数据）
 * @param  rb  环形缓冲区指针
 */
void ring_buf_clear(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
}



// 将BCD格式转换为十进制数
int BCDToDecimal(int bcd) {
    int decimal = 0;
    decimal += (bcd/10)*16;
    decimal += (bcd% 10);
    return decimal;
}

float str_to_float(const unsigned char *str) {
    float result = 0.0f;
    float decimal_factor = 1.0f;
    bool is_negative = false;
    bool is_decimal = false;

    // Skip leading whitespaces
    while (isspace(*str)) {
        str++;
    }

    // Handle optional sign
    if (*str == '-' || *str == '+') {
        if (*str == '-') {
            is_negative = true;
        }
        str++;
    }

    // Convert the integral and fractional parts
    while (*str) {
        if (isdigit(*str)) {
            if (is_decimal) {
                decimal_factor *= 0.1f;
                result += (*str - '0') * decimal_factor;
            } else {
                result = result * 10.0f + (*str - '0');
            }
        } else if (*str == '.') {
            if (is_decimal) {
                // Multiple decimal points found, invalid input
                break;
            }
            is_decimal = true;
        } else {
            // Invalid character found, stop parsing
            break;
        }
        str++;
    }

    if (is_negative) {
        result = -result;
    }

    return result;
}

//字符串转整形
<<<<<<< HEAD
uint32_t hex_string_to_int(const unsigned char *str)
{
    uint32_t result = 0;
    uint8_t  nibble;

    if (str == NULL) {
        return 0;
    }

    /* 跳过可选前缀 "0x" / "0X" */
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (*str != '\0')
    {
        char c = *str;

        if (c >= '0' && c <= '9') {
            nibble = (uint8_t)(c - '0');
        }
        else if (c >= 'a' && c <= 'f') {
            nibble = (uint8_t)(c - 'a' + 10);
        }
        else if (c >= 'A' && c <= 'F') {
            nibble = (uint8_t)(c - 'A' + 10);
        }
        else {
            /* 遇到非法字符，停止解析 */
            break;
        }

        result = (result << 4) | nibble;
        str++;
    }

    return result;
=======
unsigned int hex_string_to_int(const unsigned char *hex_str) {
	unsigned int result = 0;
	size_t len = strlen((const char*)hex_str);

	// 遍历字符串中的每个字符
	for (size_t i = 0; i < len; ++i) {
		char c = hex_str[i];

		// 将大写字母转换为小写字母（可选，但为了统一处理）
		c = tolower(c);

		// 检查字符是否为有效的十六进制数字
		if (c >= '0' && c <= '9') {
			result = (result << 4) + (c - '0');
		} else if (c >= 'a' && c <= 'f') {
			result = (result << 4) + (c - 'a' + 10);
		} else {
			// 如果字符不是有效的十六进制数字，则返回错误或设置一个标志
			fprintf(stderr, "Invalid hexadecimal digit: %c\n", c);
			return 0; // 或你可以设置一个错误代码
		}
	}
	return result;
>>>>>>> 974e0862a7e52c0771c8a8e840632994fe4bc83a
}


double round_to_two_decimal_places(float number) 
{
	// 使用double进行乘法，以减少精度损失  
    double temp = number * 100.0;  
    // 直接截断小数部分，因为(long)转换会去掉小数部分  
    long truncated = (long)temp; 
	double res=(truncated)/100.0;
	
  return res;
}

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

