#include "h2s.h"

unsigned char H2Sreadbuff[] = {0x43, 0x0D, 0x0A};//'C'

void H2Srcv(void)
{
	unsigned char peek_buf[h2sLengthMax + 1] = {0};
	unsigned char frame[h2sLengthMax] = {0};
	unsigned short frame_len = 0;
    unsigned short avail = 0;
    unsigned short n, i;

	avail = ring_buf_available(&rb_usart2);
    if (avail == 0) return;

    // 一次最多查看 LINE_MAX_LEN 字节
    n = (avail > h2sLengthMax) ? h2sLengthMax : avail;
    ring_buf_peek_n(&rb_usart2, peek_buf, n);

    // 在查看的数据里找 '\n'
    for (i = 0; i < n; i++) 
	{
        if (peek_buf[i] == '\n') 
		{
            // 找到帧尾，帧长 = i + 1（包含 '\n'）
            frame_len = i + 1;

            // 一次性读出整帧
            ring_buf_read(&rb_usart2, frame, frame_len);

            // 去掉 '\r' 和 '\n'，解析
            if (frame_len > 0 && frame[frame_len - 1] == '\n') frame_len--;
            if (frame_len > 0 && frame[frame_len - 1] == '\r') frame_len--;
            frame[frame_len] = '\0';

            // 检查帧头是否是 'C'
            if (frame_len > 0 && frame[0] == 'C') 
			{
                praseH2SData(frame);
				ring_buf_clear(&rb_usart2);
            }
            return;   // 处理完一帧就退出，下次再处理下一帧
        }
    }

    // 没找到 '\n'，说明数据还不完整，等下次
    // 但要注意：如果 n == LINE_MAX_LEN 还没找到 '\n'，说明帧太长了
    if (n == h2sLengthMax) {
        // 缓冲区里没有换行符且已经到达最大长度，丢弃最旧的数据
        ring_buf_drop(&rb_usart2);
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


