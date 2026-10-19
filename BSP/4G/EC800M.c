#include "EC800M.h"

stu4GDef stu_4G = {0};

extern UART_HandleTypeDef huart1;       /* 4G 模块串口*/
static unsigned char EC800M_UART_Send(const uint8_t *data, uint16_t len);
static unsigned char EC800M_Publish  (const char *topic, const char *payload);

/* 全局共用缓冲区（在 ec800m.c 顶部） */
uint8_t  tmp[RING_BUF_SIZE];
char Topic[100];
char json_str[300];
//char payload[280];
/* ============================================================================
 *                              接收解析
 * ========================================================================== */

/**
 * @brief  从 rb_usart1 搬数据到 stu_4G.From4G，检测 \r\n
 * @note   你的 HAL_UARTEx_RxEventCallback 已经负责把数据写入 rb_usart1，
 *         这里只负责"消费"它。
 */
void EC800M_Poll_Rx(void)
{
    uint16_t n = ring_buf_read(&rb_usart1, tmp, sizeof(tmp));
    static unsigned char Data[2] = {0};
#if (DEBUG_UART5 == 1)
    static bool rcv_prefix_printed = false;   /* 本帧是否已打过 "rcv: " */
#endif

    for (uint16_t i = 0; i < n; i++)
    {
        unsigned char ch = tmp[i];

        Data[0] = Data[1];
        Data[1] = ch;

        if (stu_4G.From4G_N < FROM4G_BUF_SIZE) {
            stu_4G.From4G[stu_4G.From4G_N++] = (char)ch;
        }

#if (DEBUG_UART5 == 1)
        /* ---------- 调试打印 rcv ---------- */
        if (!rcv_prefix_printed) {
            printf("rcv: ");
            rcv_prefix_printed = true;
        }
        printf("%c", ch);

        /* 遇到 \n 认为一帧结束，下帧重新打 "rcv: " */
        if (ch == '\n') {
            rcv_prefix_printed = false;
        }
#endif

        if (Data[0] == 0x0D && Data[1] == 0x0A) {
            stu_4G.isFrom4G_OK++;
        }
    }
}

/**
 * @brief  4G 串口发送（阻塞等待上一次发送完成，然后走 uartSendIT）
 * @retval 1=成功，0=失败
 */
static unsigned char EC800M_UART_Send(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) return 0;

    /* 等上一次中断发送完成（最长 1s） */
	for (int i = 0; i < 200; i++) 
	{     // 200 * 5ms = 1000ms
		if (huart1.gState == HAL_UART_STATE_READY) break;
		osDelay(5);
	}

    return (uartSendIT(1, data, len) == HAL_OK) ? 1 : 0;   /* 用串口1发送 */
}

/* ============================================================================
 *                              AT 命令状态机
 * ========================================================================== */

/**
 * @brief  发送 AT 命令并等待一个关键字
 */
unsigned char AT_Send(char *str, char *Find, unsigned char errN)
{
    AT_State state = STATE_SEND_CMD;
    int      retry = 0;

    while (retry < errN)
    {
        switch (state)
        {
			case STATE_SEND_CMD:
				ring_buf_clear(&rb_usart1);
				memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
				stu_4G.From4G_N    = 0;
				stu_4G.isFrom4G_OK = 0;
#if (DEBUG_UART5 == 1)
                printf("send: %s", str);
#endif
                if (EC800M_UART_Send((uint8_t *)str, strlen(str)) == 0) {
                    retry++;
                    break;
                }

                /* 发送成功后再清接收侧（避免误删模块刚回的响应） */
                ring_buf_clear(&rb_usart1);
                memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
                stu_4G.From4G_N   = 0;
                stu_4G.isFrom4G_OK = 0;

                state = STATE_WAIT_RESPONSE;
                break;

			case STATE_WAIT_RESPONSE:
				for (int i = 0; i < 20; i++) {          // 最多 2000ms
					EC800M_Poll_Rx();

					/* 只有当接收缓冲里真的出现关键字才提前结束 */
					if (stu_4G.From4G_N > 0 &&
						strstr((const char *)stu_4G.From4G, (const char *)Find)) {
						return 1;                        // 成功，直接返回
					}
					osDelay(100);                        // 让出 CPU
				}
				state = STATE_CHECK_RESPONSE;            // 2s 到，再去判断一次
                break;

            case STATE_CHECK_RESPONSE:
                EC800M_Poll_Rx();
                if (strstr((const char *)stu_4G.From4G, (const char *)Find)) {
                    return 1;
                } 
				/* 没匹配上，清状态避免下一轮误判 */
				stu_4G.isFrom4G_OK = 0;
				stu_4G.From4G_N    = 0;
				memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
				retry++;
				state = STATE_SEND_CMD;
                break;

            default: break;
        }
    }
    return 0;
}

/**
 * @brief  发送 AT 命令并等待两个关键字同时出现
 * @param  str    要发送的 AT 命令
 * @param  Find1  关键字 1
 * @param  Find2  关键字 2
 * @param  errN   最大重试次数
 * @retval 1=匹配成功，0=失败
 */
unsigned char AT_Send_2(char *str, char *Find1, char *Find2, unsigned char errN)
{
    AT_State state = STATE_SEND_CMD;
    int      retry = 0;

    while (retry < errN)
    {
        switch (state)
        {
			case STATE_SEND_CMD:
				ring_buf_clear(&rb_usart1);
				memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
				stu_4G.From4G_N    = 0;
				stu_4G.isFrom4G_OK = 0;
#if (DEBUG_UART5 == 1)
                printf("send: %s", str);
#endif

                if (EC800M_UART_Send((uint8_t *)str, strlen(str)) == 0) {
                    retry++;
                    break;
                }

                ring_buf_clear(&rb_usart1);
                memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
                stu_4G.From4G_N   = 0;
                stu_4G.isFrom4G_OK = 0;

                state = STATE_WAIT_RESPONSE;
                break;

            case STATE_WAIT_RESPONSE:
                for (int i = 0; i < 20; i++) {          // 最多 2000ms
                    EC800M_Poll_Rx();

                    /* 只有两个关键字都出现才提前结束，否则继续等 */
                    if (stu_4G.From4G_N > 0 &&
                        strstr((const char *)stu_4G.From4G, (const char *)Find1) &&
                        strstr((const char *)stu_4G.From4G, (const char *)Find2)) {
                        return 1;                       // 匹配成功，直接返回
                    }
                    osDelay(100);                       // 让出 CPU
                }
                state = STATE_CHECK_RESPONSE;           // 2s 到，最后再判一次
                break;

            case STATE_CHECK_RESPONSE:
                EC800M_Poll_Rx();
                if (strstr((const char *)stu_4G.From4G, (const char *)Find1) &&
                    strstr((const char *)stu_4G.From4G, (const char *)Find2)) {
                    return 1;
                } 
				/* 没匹配上，清状态，避免下一轮误判 */
                stu_4G.isFrom4G_OK = 0;
                stu_4G.From4G_N    = 0;
                memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
                retry++;
                state = STATE_SEND_CMD;
                break;

            default: break;
        }
    }
    return 0;
}

/* ============================================================================
 *                          EC800M 两步式发布消息
 * ========================================================================== */

/**
 * @brief  EC800M 发布 MQTT 消息（AT+QMTPUBEX 两步式）
 * @retval 1=成功，0=失败
 */
static unsigned char EC800M_Publish(const char *topic, const char *payload)
{
    int  len = (int)strlen(payload);

    /* 步骤 1：发命令 + 长度 */
    snprintf(stu_4G.AT_Buff, sizeof(stu_4G.AT_Buff),
             "AT+QMTPUBEX=0,1,1,0,\"%s\",%d\r\n", topic, len);
#if (DEBUG_UART5 == 1)
    printf("send: %s", stu_4G.AT_Buff);
#endif

    ring_buf_clear(&rb_usart1);
    memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
    stu_4G.From4G_N   = 0;
    stu_4G.isFrom4G_OK = 0;

    if (EC800M_UART_Send((uint8_t *)stu_4G.AT_Buff, strlen(stu_4G.AT_Buff)) == 0) 
		return 0;

    /* 等待 '>' 提示符 */
	for (int i = 0; i < 60; i++) 
	{     // 60 * 50ms = 3000ms
		EC800M_Poll_Rx();
		if (strchr(stu_4G.From4G, '>')) 
			break;
		osDelay(50);
	}

    if (!strchr(stu_4G.From4G, '>')) 
		return 0;

	/*清掉步骤 1 的缓冲，避免下面匹配到命令回显 */
    ring_buf_clear(&rb_usart1);
    memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
    stu_4G.From4G_N    = 0;
    stu_4G.isFrom4G_OK = 0;

    /* 步骤 2：发 payload */
#if (DEBUG_UART5 == 1)
	printf("send payload(%d): %s\n", len, payload);
#endif
    if (EC800M_UART_Send((uint8_t *)payload, len) == 0) 
		return 0;

    /* 等待发布结果 */
	for (int i = 0; i < 160; i++) 
	{     // 160 * 50ms = 8000ms
		EC800M_Poll_Rx();
		if (strstr((const char *)stu_4G.From4G, "ERROR"))    
		{
			return 0;
		}
		if (strstr(stu_4G.From4G, "+QMTPUBEX")) 
		{
			if (strstr((const char *)stu_4G.From4G, ",0\r\n"))
			{
				return 1;
			}
            break;
        }
		osDelay(50);
	}

    return 0;
}

/* ============================================================================
 *                          获取模块 ICCID / SN
 * ========================================================================== */

uint8_t GET_4G_ID_SN(void)
{
    unsigned char Result = 0;
    unsigned char i, j;

    /* ---------- 获取 ICCID（EC800M 用 AT+QCCID） ---------- */
    Result = AT_Send("AT+QCCID\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    if (Result == 1)
    {
        j = 0;
        memset(stu_4G.mqttClientID, 0, sizeof(stu_4G.mqttClientID));
        for (i = 10; i < 32; i++)
        {
            if ((stu_4G.From4G[i] >= '0' && stu_4G.From4G[i] <= '9') ||
                (stu_4G.From4G[i] >= 'A' && stu_4G.From4G[i] <= 'Z'))
            {
                stu_4G.mqttClientID[j++] = stu_4G.From4G[i];
            }
        }
    }

    /* ---------- 获取 IMEI/SN ---------- */
    Result = AT_Send("AT+CGSN=1\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    if (Result == 1)
    {
        j = 0;
        memset(stu_4G.mqttClientSN, 0, sizeof(stu_4G.mqttClientSN));
        for (i = 0; i < stu_4G.From4G_N; i++)
        {
            if (stu_4G.From4G[i] >= '0' && stu_4G.From4G[i] <= '9')
            {
                stu_4G.mqttClientSN[j++] = stu_4G.From4G[i];
            }
        }
    }

    return 1;

Mqtt_Fail:
    AT_Send("AT+CFUN=1,1\r\n", "OK", 2);
    return 0;
}

/* ============================================================================
 *                              4G 初始化
 * ========================================================================== */

unsigned char Init_4G(void)
{
    unsigned char Result = 0;
    unsigned char buff[20];
    uint8_t       count  = 20;

    /* ---------- 1. AT 测试 ---------- */
    Result = AT_Send("AT\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

	Result = AT_Send("ATE0\r\n", "OK", 8);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 2. 获取 ICCID / SN ---------- */
    GET_4G_ID_SN();

    /* ---------- 3. 查询信号质量 ---------- */
    stu_4G.notsigal = true;
    do {
        if (count > 20) break;

        Result = AT_Send("AT+CSQ\r\n", "CSQ", 10);
        if (Result == 0) goto Mqtt_Fail;

        if (Result == 1)
        {
            memset(buff, 0, sizeof(buff));
            if (strstr((const char *)stu_4G.From4G, "+CSQ: "))
            {
                char *index = strstr((const char *)stu_4G.From4G, "+CSQ: ");
                buff[0] = *(index + 6);
                buff[1] = *(index + 7);
                stu_4G.CSQ = atoi((const char *)buff);
            }
        }
        if ((stu_4G.CSQ > 1) && (stu_4G.CSQ < 99)) {
            stu_4G.notsigal = false;
            break;
        }
    } while (count--);

    if (stu_4G.notsigal) return 0;

    if (stu_4G.CSQ == 99 || stu_4G.CSQ < 12) {
        stu_4G.CSQ = 15;
    }

    /* ---------- 4. 断开旧的 MQTT 连接 ---------- */
//    AT_Send("AT+QMTDISC=0\r\n",  "OK", 1);
//    osDelay(1000);
    AT_Send("AT+QMTCLOSE=0\r\n", "OK", 1);
    osDelay(1000);

    /* ---------- 5. 射频 / 网络注册 ---------- */
    Result = AT_Send("AT+CFUN=1\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;
    Result = AT_Send("AT+CEREG?\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;
    Result = AT_Send("AT+CREG=1\r\n",  "OK", 20); if (Result == 0) goto Mqtt_Fail;

    /* ---------- 6. 配置 MQTT 会话 ---------- */
	sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"version\",0,4\r\n");
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;
    sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"recv/mode\",0,0,1\r\n");
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;
	sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"keepalive\",0,60\r\n");
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;
    sprintf(stu_4G.AT_Buff, "AT+QMTCFG=\"onenet\",0,\"%s\",\"%s\"\r\n",
                    stu_flash.productKey,
                    stu_flash.deviceSecret);
    Result = AT_Send(stu_4G.AT_Buff, "OK", 5);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 7. 打开 MQTT 网络 ---------- */
    memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
    sprintf(stu_4G.AT_Buff, "AT+QMTOPEN=0,\"%s\",1883\r\n", stu_flash.mqttBroker);
    Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTOPEN", 8);
    if (Result == 0) goto Mqtt_Fail;

    /* ---------- 8. 生成 OneNET Token 并连接 ---------- */
    memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
    sprintf(stu_4G.AT_Buff, "AT+QMTCONN=0,\"%s\"\r\n",
            stu_flash.deviceName);
    Result = AT_Send_2(stu_4G.AT_Buff, "OK", "QMTCONN", 8);
    if (Result == 0) goto Mqtt_Fail;

    if (Result == 1)
    {
        /* 检查 +QMTCONN: 0,0,x 中的 x，0=成功，其它=失败 */
		if (strstr((const char *)stu_4G.From4G, "+QMTCONN: 0,0,0") != NULL)
		{
			stu_4G.InitFlag = 1;
			return 1;
		}
		else
		{
			/* 比如 +QMTCONN: 0,0,4 */
			goto Mqtt_Fail;
		}
    }

Mqtt_Fail:
    stu_4G.InitFlag = 0;
    AT_Send("AT+CFUN=1,1\r\n", "OK", 2);
    return 0;
}

/* ============================================================================
 *                          MQTT 业务：订阅影子
 * ========================================================================== */

void Mq_Sub_shadow(void)
{
    const char *shadow_payload = "{\"id\":\"12\",\"version\":\"1.0\",\"params\":[\"HeartFreq\",\"Latitude\",\"Longitude\"]}";       

    unsigned char uchFlashCnt = 0;

    if (!stu_4G.get_sub1_flg)
    {
        memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
        memset(Topic, 0, sizeof(Topic));
        sprintf(Topic, "$sys/%s/%s/thing/property/desired/get/reply",
                stu_flash.productKey, stu_flash.deviceName);
        sprintf(stu_4G.AT_Buff, "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic);
        AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8);
        stu_4G.get_sub1_flg = true;
    }

    memset(Topic, 0, sizeof(Topic));
    sprintf(Topic, "$sys/%s/%s/thing/property/desired/get",
            stu_flash.productKey, stu_flash.deviceName);

    if (EC800M_Publish(Topic, (const char *)shadow_payload) == 0) {
        return;
    }

	/* 先看 From4G 里是不是已经有了（同一帧一起到） */
	if (strstr((const char *)stu_4G.From4G, "+QMTRECV") == NULL)
	{
		/* 单独等 +QMTRECV，最多 5 秒 */
		ring_buf_clear(&rb_usart1);
		memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
		stu_4G.From4G_N    = 0;
		stu_4G.isFrom4G_OK = 0;

		for (int i = 0; i < 100; i++) // 100 * 50ms = 5s
		{           
		   EC800M_Poll_Rx();
		   if (strstr((const char *)stu_4G.From4G, "+QMTRECV"))
			   break;
		   osDelay(50);
		}
	}
	if (!strstr((const char *)stu_4G.From4G, "+QMTRECV"))
	{
        return;                                // 服务器没下发，算了
	}

    char *result = extract_content(stu_4G.From4G);
    if (result != NULL)
    {
        if (get_json_value(result) == 0) {
            if (stu_flash.updateIntervalmins != stu_flashjson.updateIntervalmins) {
                stu_flash.updateIntervalmins = stu_flashjson.updateIntervalmins;
                uchFlashCnt++;
            }
        }
        if (get_json_value1(result) == 0) {
            if (stu_flash.unLat != stu_flashjson.unLat) {
                stu_flash.unLat = stu_flashjson.unLat;
                stu_flash.dbLat = stu_flashjson.dbLat;
                uchFlashCnt++;
            }
        }
        if (get_json_value2(result) == 0) {
            if (stu_flash.unLon != stu_flashjson.unLon) {
                stu_flash.unLon = stu_flashjson.unLon;
                stu_flash.dbLon = stu_flashjson.dbLon;
                uchFlashCnt++;
            }
        }
        if (uchFlashCnt != 0) {
            flashWriteToEEPROM();
            osDelay(4000);
        }
        memset(stu_4G.From4G, 0, sizeof(stu_4G.From4G));
        free(result);
    }
}

/* ============================================================================
 *                          MQTT 业务：上报属性
 * ========================================================================== */

void Mq_Pub_P(void)
{
    osDelay(1000);

    if (!stu_4G.get_sub2_flg)
    {
        memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
        memset(Topic, 0, sizeof(Topic));

        snprintf(Topic, sizeof(Topic),
                 "$sys/%s/%s/thing/property/post/reply",
                 stu_flash.productKey, stu_flash.deviceName);

        snprintf(stu_4G.AT_Buff, sizeof(stu_4G.AT_Buff),
                 "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic);

        if (AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8) == 0) {
            return;
        }
        stu_4G.get_sub2_flg = true;
    }

    /* ---------- 数据限幅与处理 ---------- */
    if (stu_sensor.usCH4 > 100) stu_sensor.usCH4 = 100;

    stu_sensor.usH2S = getusData(stu_sensor.sH2S);
    if (stu_sensor.usH2S > 100) stu_sensor.usH2S = 100;

    stu_sensor.usLD = getusData(stu_sensor.sLD);
    if (stu_sensor.usLD == 0) stu_sensor.uchLDCnt++;
    else                      stu_sensor.uchLDCnt = 0;

	stu_sensor.usTemp = getsData(stu_sensor.sTemp);
    if (stu_sensor.usTemp > 1000) stu_sensor.usTemp = 1000;
    else if (stu_sensor.usTemp < -400)   stu_sensor.usTemp = -400;

	stu_sensor.usHum = getusData(stu_sensor.sHum);
    if (stu_sensor.usHum > 1000) stu_sensor.usHum = 1000;

	stu_sensor.usAngle = getusData(stu_sensor.sAngle);
    if (stu_sensor.usAngle > 1800) stu_sensor.usAngle = 1800;

    if (stu_sensor.uchBattery > 100) stu_sensor.uchBattery = 100;

    if (stu_flash.dbLon < 0)   stu_flash.dbLon = 0;
    if (stu_flash.dbLon > 180) stu_flash.dbLon = 180;

    if (stu_flash.dbLat < 0)   stu_flash.dbLat = 0;
    if (stu_flash.dbLat > 90)  stu_flash.dbLat = 90;

    /* ---------- 自己拼 JSON ---------- */
    int json_len = snprintf(json_str, sizeof(json_str),
        "{\"id\":\"12\",\"version\":\"1.0\",\"params\":{"
        "\"CH4\":{\"value\":%u},"
        "\"H2S\":{\"value\":%u},"
		"\"Temp\":{\"value\":%d},"
		"\"Humidity\":{\"value\":%u},"
		"\"Angle\":{\"value\":%u},"
        "\"Radar\":{\"value\":%u},"
        "\"Battery\":{\"value\":%u},"
        "\"Longitude\":{\"value\":%.6f},"
        "\"Latitude\":{\"value\":%.6f}"
        "}}",
        stu_sensor.usCH4,
        stu_sensor.usH2S,
		stu_sensor.usTemp,
		stu_sensor.usHum,
		stu_sensor.usAngle,
        stu_sensor.usLD,
        stu_sensor.uchBattery,
        stu_flash.dbLon,
        stu_flash.dbLat);

    if (json_len < 0 || json_len >= (int)sizeof(json_str)) {
        /* 缓冲区不够或格式化失败 */
        return;
    }

    /* ---------- 拼 topic ---------- */
    memset(Topic, 0, sizeof(Topic));
    snprintf(Topic, sizeof(Topic),
             "$sys/%s/%s/thing/property/post",
             stu_flash.productKey, stu_flash.deviceName);

    /* ---------- 发布条件 ---------- */
    if ((stu_sensor.uchLDCnt == 0) || (stu_sensor.uchLDCnt == 3))
    {
        EC800M_Publish(Topic, json_str);
		osDelay(1000);
		stu_4G.process = 4;
        if (stu_sensor.uchLDCnt == 3) {
            stu_sensor.uchLDCnt = 0;
        }
    }
}

/* ============================================================================
 *                          MQTT 业务：上报开盖状态
 * ========================================================================== */

void Mq_Pub_open(void)
{
    osDelay(1000);

    if (!stu_4G.get_sub2_flg)
    {
        memset(stu_4G.AT_Buff, 0, sizeof(stu_4G.AT_Buff));
        memset(Topic, 0, sizeof(Topic));
        sprintf(Topic, "$sys/%s/%s/thing/property/post/reply",
                stu_flash.productKey, stu_flash.deviceName);
        sprintf(stu_4G.AT_Buff, "AT+QMTSUB=0,1,\"%s\",1\r\n", Topic);
        if (AT_Send_2(stu_4G.AT_Buff, "OK", "+QMTSUB", 8) == 0) {
            return;
        }
        stu_4G.get_sub2_flg = true;
    }

    /* ---------- 手写 JSON ---------- */
    int json_len = snprintf(json_str, sizeof(json_str),
        "{\"id\":\"12\",\"version\":\"1.0\",\"params\":{"
        "\"CoverStatus\":{\"value\":%s}"
        "}}",
        stu_flag.bAccFlag ? "true" : "false");

    if (json_len < 0 || json_len >= (int)sizeof(json_str)) {
#if (DEBUG_UART5 == 1)
        printf("json snprintf failed, ret=%d\n", json_len);
#endif
        return;
    }

    /* ---------- 拼 topic ---------- */
    memset(Topic, 0, sizeof(Topic));
    snprintf(Topic, sizeof(Topic),
             "$sys/%s/%s/thing/property/post",
             stu_flash.productKey, stu_flash.deviceName);

#if (DEBUG_UART5 == 1)
    printf("publish topic=%s\n", Topic);
    printf("publish payload(%d)=%s\n", json_len, json_str);
#endif

    EC800M_Publish(Topic, json_str);
}

/* ============================================================================
 *                              JSON 解析
 * ========================================================================== */

char* extract_content(const char* str)
{
    char *start = strchr(str, '{');
    if (start == NULL) return NULL;

    char *end = strrchr(str, '}');
    if (end == NULL) return NULL;

    int length = end - start + 1;

    char* middle = (char*)malloc((length + 1) * sizeof(char));
    if (middle == NULL) return NULL;

    strncpy(middle, start, length);
    middle[length] = '\0';
    return middle;
}

/**
 * @brief  从 json 中提取 "outer_key" 后面 "inner_key" 的数值
 * @param  json       整个 +QMTRECV 响应（或从 '{' 开始的 JSON）
 * @param  outer_key  外层 key，如 "HeartFreq"
 * @param  inner_key  内层 key，如 "value"
 * @param  out        输出数值
 * @retval 0=成功，-1=失败
 */
int json_get_nested_value(const char *json,
                          const char *outer_key,
                          const char *inner_key,
                          double     *out)
{
    if (json == NULL || outer_key == NULL || inner_key == NULL || out == NULL)
        return -1;

    char pattern[40];

    /* ★ 只从 +QMTRECV 之后开始找，避免误命中回显/别处的字符串 */
    const char *urc = strstr(json, "+QMTRECV");
    if (urc != NULL) json = urc;

    /* 1) 找 "outer_key" */
    snprintf(pattern, sizeof(pattern), "\"%s\"", outer_key);
    const char *p = strstr(json, pattern);
    if (p == NULL) return -1;

    /* 2) 在 outer_key 之后再找 "inner_key" */
    snprintf(pattern, sizeof(pattern), "\"%s\"", inner_key);
    p = strstr(p, pattern);
    if (p == NULL) return -1;

    /* 3) 跳过冒号 + 空白 */
    p = strchr(p, ':');
    if (p == NULL) return -1;
    p++;
    while (*p == ' ' || *p == '\t') p++;

    /* 4) 解析数值 */
    char *end = NULL;
    double v = strtod(p, &end);
    if (end == p) return -1;      /* 没解析出任何数字 */

    *out = v;
    return 0;
}

/**
 * @brief  解析 HeartFreq.value → 上报间隔
 * @retval 0=成功，1=失败
 */
int get_json_value(const char *json_str)
{
    double v;
    if (json_get_nested_value(json_str, "HeartFreq", "value", &v) != 0)
        return 1;

    int cnt = (int)v;
    if (cnt < 2)  cnt = 2;
    if (cnt > 24) cnt = 24;

    stu_sensor.uchReportCount        = (uint8_t)cnt;
    stu_flashjson.updateIntervalmins = (uint16_t)(24 * 60 / cnt);
    return 0;
}

/**
 * @brief  解析 Latitude.value
 */
int get_json_value1(const char *json_str)
{
    double v;
    if (json_get_nested_value(json_str, "Latitude", "value", &v) != 0)
        return 1;

    stu_flashjson.dbLat = v;
    stu_flashjson.unLat = (unsigned int)(v * 10000);
    return 0;
}

/**
 * @brief  解析 Longitude.value
 */
int get_json_value2(const char *json_str)
{
    double v;
    if (json_get_nested_value(json_str, "Longitude", "value", &v) != 0)
        return 1;

    stu_flashjson.dbLon = v;
    stu_flashjson.unLon = (unsigned int)(v * 10000);
    return 0;
}



